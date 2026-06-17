#ifndef REPAIR_H
#define REPAIR_H

#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <fstream>
#include <unordered_map>
#include "../include/queue.h"
#include "../include/threaded_sequence.h"
#include "../include/utils.h"

using namespace std;


class Repair
{
private:
    QUEUE q;
    unordered_map<pair<st, st>, PAIR*, PairHash> ht;
    TSEQ seq;
    st rule;
    vector<st> ruleHistory;

    void findAdjPairs(vector<st>& pairPos, st position);
    void decrease(st pos);
    void replace(st position);
    void insert(PAIR* p, st current_position);
    void firstPass(bool verbose);
    void compactify();
    void compress(bool verbose);
    void printHashTable();
    void removeFromOccList(PAIR* p, st pos);

public:
    
    Repair(const string& input);
    ~Repair();
    void run(bool verbose = false, bool verbose2 = false)
    {
        if(verbose) cout << "[VERBOSE] Running first pass...\n";
        firstPass(verbose2);
        if(verbose) cout << "[VERBOSE] Compressing...\n";
        compress(verbose2);
        if(verbose) cout << "[VERBOSE] Compression ended.\n[VERBOSE] Encoding...";

    }

    TSEQ getSequence() { return this->seq;}
    vector<size_t> getRuleHistory() { return this->ruleHistory;}
    void output(bool verbose = false);
    void print();
    void encode();
    template<typename T> void serialize(string filename, const T& serialized);
};

 

inline void Repair::compactify()
{
    vector<SEQ> seq_new;

    for (st i = 0; i < seq.size(); i++) {
        if (seq[i].code != N) {
            seq_new.push_back(seq[i]);
        } 
        else if (seq[i].next < seq.size()) { 
            i = seq.next(i) - 1;
        }
    }

    st newStringSize = seq_new.size();
    st oldStringSize = seq.size();

    unordered_map<st, st> posMap;

    st oldPos = 0;
    st newPos = 0;

    while (oldPos < oldStringSize && newPos < newStringSize) {
        if (seq[oldPos].code != N) {
            posMap[oldPos] = newPos;
            newPos++;
        }
        oldPos++;
    }

    for (auto& pair_entry : ht) {
        PAIR* p = pair_entry.second;

        if (posMap.count(p->f_pos))
            p->f_pos = posMap[p->f_pos];

        if (posMap.count(p->b_pos))
            p->b_pos = posMap[p->b_pos];
    }

    for (st i = 0; i < seq_new.size(); i++) {
        if (seq_new[i].prev != N && posMap.count(seq_new[i].prev)) {
            seq_new[i].prev = posMap[seq_new[i].prev];
        } else {
            seq_new[i].prev = N;
        }

        if (seq_new[i].next != N && posMap.count(seq_new[i].next)) {
            seq_new[i].next = posMap[seq_new[i].next];
        } else {
            seq_new[i].next = N;
        }
    }

    seq = std::move(seq_new);
}

inline void Repair::printHashTable()
{
    for (const auto &entry : ht)
    {
        cout << "(" << entry.first.first << ", " << entry.first.second << ") "
             << "-> freq: " << entry.second->freq
             << ", f_pos: " << (entry.second->f_pos == N ? "NULL" : to_string(entry.second->f_pos))
             << ", b_pos: " << (entry.second->b_pos == N ? "NULL" : to_string(entry.second->b_pos))
             << endl;
    }
}

template<typename T> inline void Repair::serialize(string filename, const T& serialized)
{
    ofstream fout;
    fout.open(filename, ios::binary | ios::out | ios::trunc);

    for(const auto& character : serialized)
    {
        if constexpr (is_same_v<T, vector<size_t>>)
        {
            fout.write(reinterpret_cast<const char*>(&character), sizeof(character));
        }
        else
        {
            if(character.code != N)
            {
                fout.write(reinterpret_cast<const char*>(&character.code), sizeof(character.code));
            }
        }
    }
}

inline void Repair::output(bool verbose)
{
    if(verbose)
    {
    cout << "\nCompressed text: ";
    for (size_t i = 0; i < seq.size(); i++)
    {
        if(seq[i].code != N)
        {
            if(seq[i].code < 128)
            {
                cout << static_cast<char>(seq[i].code);
            }
            else
            {
                cout << "["  << static_cast<size_t>(seq[i].code) << "]";
            }
        }
    }
    cout << "\nRule History: ";
    cout << ruleHistory << "\n";
    cout << "\n";
    }

    size_t c = 0;
    for (size_t i = 0; i < seq.size(); i++)
    {
        if(seq[i].code != N)
        c++;
    }
    this->serialize<TSEQ>("../output/sequence.rp", this->seq);
    this->serialize<vector<st>>("../output/rulehistory.rp", this->ruleHistory);
    cout << "\n Rule History size: " << ruleHistory.size() << "\n";
    cout << "\n Compressed text size: " << c << "\n";
    
    bool check_correctness();
}   

inline Repair::Repair(const std::string& input) : q(input), seq(input), rule(255){}

inline Repair::~Repair()
{
    for (auto& kv : ht)
    {
        delete kv.second;
    }
    ht.clear();
}

struct Paired
{
    size_t left;
    size_t right;
};

class Despair
{
    private:
    static void expand(size_t symbol, const vector<Paired>& rules, vector<size_t>& out)
    {
        
        if (symbol <= 255)
        {
            out.push_back(symbol);
        } 
        else
        {
            expand(rules[symbol - 256].left, rules, out);
            expand(rules[symbol - 256].right, rules, out);
        }
    }
    public:
    static vector<size_t> despair(const vector<size_t>& sequence, const vector<size_t>& ruleHistory)
    {
        vector<size_t> decoded_sequence;
        vector<Paired> rules;
        rules.reserve(ruleHistory.size() / 2);
        for(size_t i = 0; i < ruleHistory.size(); i += 2)
        {
            Paired p;
            p.left = ruleHistory[i];
            p.right = ruleHistory[i+1];
            rules.push_back(p);
        }
        for (size_t symbol : sequence)
        {
            expand(symbol, rules, decoded_sequence);
        }
        return decoded_sequence;
    }
};


#endif // REPAIR_H
