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
#include <type_traits>
#include "../include/queue.h"
#include "../include/threaded_sequence.h"
#include "../include/utils.h"
#include <functional>
using namespace std;


class Repair
{
private:
    QUEUE q;
    unordered_map<pair<size_t, size_t>, PAIR*, PairHash> ht;
    TSEQ seq;
    size_t rule;
    vector<size_t> ruleHistory;
    size_t alpha_size;

    void findAdjPairs(vector<st>& pairPos, st position);
    void decrease(st pos);
    void replace(st position);
    void insert(PAIR* p, st current_position);
    void firstPass(bool verbose);
    void gen(function<bool()> stop_condition, bool verbose);
    void removeFromOccList(PAIR* p, st pos);
    void writeuint(size_t s, string& bitString, size_t maxRules);
    void encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory,string& bitString, size_t& num_rules_encoded, unordered_map<size_t,size_t>& seen,size_t alpha_size);
    
    static inline int getBitsLen(size_t n) {
        if (n <= 1) return 1;
        int b = 0;
        while (n > 0) { b++; n >>= 1; }
        return b;
    }

public:
    
    Repair(const vector<size_t>& input);
    ~Repair();
    
    string compress(function<bool()> stop_condition, bool verbose = false, bool verbose2 = false);
    TSEQ getSequence() { return this->seq;}
    vector<size_t> getRuleHistory() { return this->ruleHistory;}
    QUEUE getQueue() {return this->q; }

    void output(bool verbose = false);
    string encode();
    template<typename T> void serialize(string filename, const T& serialized);
    
};

inline string Repair::compress(function<bool()> stop_condition, bool verbose, bool verbose2)
{
    if(verbose) cout << "[VERBOSE] Running first pass...\n";
    firstPass(verbose2);
    if(verbose) cout << "[VERBOSE] Compressing...\n";
    gen(stop_condition, verbose2);
    if(verbose) cout << "[VERBOSE] Compression ended.\n[VERBOSE] Encoding...\n";
    string enc = encode();
    if(verbose) cout << "[VERBOSE] Done.";
    return enc;
}

inline void Repair::writeuint(size_t s, string& bitString, size_t maxRules)
{
    int bitsLen = getBitsLen(maxRules);
    
    for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
    {
        bitString.push_back(((s >> bitPosition) & 1) ? '1' : '0');
    }
}

inline void Repair::encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory,
    string& bitString, size_t& num_rules_encoded, unordered_map<size_t,size_t>& seen,
    size_t alpha_size)
{
    auto it = seen.find(sy);
    if (sy >= alpha_size && it == seen.end())
    {
        size_t leftRule  = 2 * (sy - alpha_size);
        size_t rightRule = leftRule + 1;
        if (rightRule >= ruleHistory.size()) { cerr << "Error\n"; return; }

        bitString.push_back('1');

        size_t assignedID = num_rules_encoded++;
        seen[sy] = assignedID;

        size_t left  = ruleHistory[leftRule];
        size_t right = ruleHistory[rightRule];

        encodeCFG_rec(left,  ruleHistory, bitString, num_rules_encoded, seen, alpha_size);
        encodeCFG_rec(right, ruleHistory, bitString, num_rules_encoded, seen, alpha_size);
    }
    else
    {
        bitString.push_back('0');
        size_t val = (it != seen.end()) ? it->second : sy;
        int bitsLen = getBitsLen(num_rules_encoded);
        for (int b = bitsLen - 1; b >= 0; --b)
            bitString.push_back(((val >> b) & 1) ? '1' : '0');
    }
}

inline string Repair::encode()
{
    string bitString;
    unordered_map<size_t,size_t> seen;
    vector<size_t> _seq;
    for (const auto& c : seq)
        if (c.code != N) _seq.push_back(c.code);

    size_t totalRulesNum = ruleHistory.size() / 2;
    size_t seqLen = _seq.size();

     for (int i = 63; i >= 0; --i)
        bitString.push_back(((this->alpha_size >> i) & 1) ? '1' : '0');
    for (int i = 63; i >= 0; --i)
        bitString.push_back(((totalRulesNum >> i) & 1) ? '1' : '0');
    for (int i = 63; i >= 0; --i)
        bitString.push_back(((seqLen >> i) & 1) ? '1' : '0');
    size_t num_rules_encoded = this->alpha_size;

    for (size_t s : _seq)
        encodeCFG_rec(s, ruleHistory, bitString, num_rules_encoded, seen, this->alpha_size);

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
            cout << "["  << static_cast<size_t>(seq[i].code) << "]";
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

inline Repair::Repair(const std::vector<size_t>& input) : q(input), seq(input)
{
    size_t max_val = 0;
    for (size_t val : input)
    {
        if (val > max_val) max_val = val;
    }
    this->alpha_size = max_val + 1; 
    this->rule = this->alpha_size - 1;
}

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
    static inline int getBitsLen(size_t n) {
        if (n <= 1) return 1;
        int b = 0;
        while (n > 0) { b++; n >>= 1; }
        return b;
    }

    static size_t readuint(const string& bitString, size_t& bitPos, size_t maxRules)
    {
        int bitsLen = getBitsLen(maxRules);
        size_t value = 0;
        for (int i = 0; i < bitsLen; ++i)
        {
            value <<= 1;
            if (bitString[bitPos++] == '1') value |= 1;
        }
        return value;
    }

    static size_t decodeCFG_rec(const string& bitString, size_t& bitPos,
        vector<Paired>& rules, size_t& num_rules_encoded, size_t alpha_size)
    {
        char flag = bitString[bitPos++];
        if (flag == '1')
        {
            size_t currentID = num_rules_encoded++;
            size_t left  = decodeCFG_rec(bitString, bitPos, rules, num_rules_encoded, alpha_size);
            size_t right = decodeCFG_rec(bitString, bitPos, rules, num_rules_encoded, alpha_size);
            rules[currentID - alpha_size] = {left, right};
            return currentID;
        }
        else
        {
            int bitsLen = getBitsLen(num_rules_encoded);
            size_t value = 0;
            for (int i = 0; i < bitsLen; ++i) { value <<= 1; if (bitString[bitPos++] == '1') value |= 1; }
            return value;
        }
    }


    static void expand(size_t symbol, const vector<Paired>& rules, vector<size_t>& out, size_t alpha_size)
    {
        if (symbol - alpha_size < rules.size())
        {
            expand(rules[symbol - alpha_size].left, rules, out, alpha_size);
            expand(rules[symbol - alpha_size].right, rules, out, alpha_size);
        } 
        else
        {
            out.push_back(symbol);
        }
    }

public:

    static vector<size_t> decompress(const string& bitString)
    {
        size_t bitPos = 0;
        const size_t totalBits = bitString.size();

        auto read64 = [&]() -> size_t
        {
            size_t v = 0;
            for (int i = 0; i < 64; i++) { v <<= 1; if (bitString[bitPos++] == '1') v |= 1; }
            return v;
        };

        size_t alpha_size = read64();
        size_t totalRulesNum = read64();
        size_t seqLen = read64();

        vector<Paired> rules(totalRulesNum);
        size_t num_rules_encoded = alpha_size;

        vector<size_t> compressed_sequence;
        for (size_t i = 0; i < seqLen; i++)
        {
            compressed_sequence.push_back(decodeCFG_rec(bitString, bitPos, rules, num_rules_encoded, alpha_size));
        }

        vector<size_t> decompressed;
        for (size_t s : compressed_sequence) expand(s, rules, decompressed, alpha_size);
        return decompressed;
    }
};


#endif // REPAIR_H