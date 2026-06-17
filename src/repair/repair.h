#ifndef REPAIR_H
#define REPAIR_H

#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
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
    void compress(bool verbose);
    void removeFromOccList(PAIR* p, st pos);
    void writeuint(size_t s, string& bitString, size_t maxRules);
    void encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory, string& bitString, size_t maxRules, unordered_set<size_t>& seen);
    
public:
    
    Repair(const string& input);
    ~Repair();
    
    void run(bool verbose = false, bool verbose2 = false);
    TSEQ getSequence() { return this->seq;}
    vector<size_t> getRuleHistory() { return this->ruleHistory;}
    void output(bool verbose = false);
    string encode();
    template<typename T> void serialize(string filename, const T& serialized);
    
};

inline void Repair::run(bool verbose, bool verbose2)
{
    if(verbose) cout << "[VERBOSE] Running first pass...\n";
    firstPass(verbose2);
    if(verbose) cout << "[VERBOSE] Compressing...\n";
    compress(verbose2);
    if(verbose) cout << "[VERBOSE] Compression ended.\n[VERBOSE] Encoding...";
    encode()
    if(verbose) cout << "[VERBOSE] Done.";
    
}

inline void Repair::writeuint(size_t s, string& bitString, size_t maxRules)
{
    int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;
    
    for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
    {
        bitString.push_back(((s >> bitPosition) & 1) ? '1' : '0');
    }
}

inline void Repair::encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory, string& bitString, size_t maxRules, unordered_set<size_t>& seen)
{
    if (sy >= 256 && seen.find(sy) == seen.end())
    {
        seen.insert(sy);
        size_t leftRule = 2 * (sy - 256);
        size_t rightRule = 2 * (sy - 256) + 1;

        if (rightRule >= ruleHistory.size())
        {
            cerr << "Error: Invalid rule index for symbol " << sy << endl;
            return;
        }
        
        bitString.push_back('1');
        
        size_t left = ruleHistory[leftRule];
        size_t right = ruleHistory[rightRule];
        
        encodeCFG_rec(left, ruleHistory, bitString, maxRules, seen);
        encodeCFG_rec(right, ruleHistory, bitString, maxRules, seen);
    }
    else
    {   
        bitString.push_back('0');
        writeuint(sy, bitString, maxRules);
    }
}

inline string Repair::encode()
{
    string bitString;
    unordered_set<size_t> seen;
    vector<size_t> _seq;
    for(const auto& c : seq)
    {
        if(c.code != N)
        {
            _seq.push_back(c.code);
        }
    }
    size_t maxRules = 256 + (ruleHistory.size() / 2); 

    for (size_t s : _seq)
    {
        encodeCFG_rec(s, ruleHistory, bitString, maxRules, seen);
    }
    return bitString;
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
