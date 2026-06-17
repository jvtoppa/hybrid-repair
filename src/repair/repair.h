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

using namespace std;


class Repair
{
private:
    QUEUE q;
    unordered_map<pair<st, st>, PAIR*, PairHash> ht;
    TSEQ seq;
    st rule;
    vector<st> ruleHistory;
    size_t alpha_size;

    void findAdjPairs(vector<st>& pairPos, st position);
    void decrease(st pos);
    void replace(st position);
    void insert(PAIR* p, st current_position);
    void firstPass(bool verbose);
    void gen(bool verbose);
    void removeFromOccList(PAIR* p, st pos);
    void writeuint(size_t s, string& bitString, size_t maxRules);
    void encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory, string& bitString, size_t maxRules, unordered_set<size_t>& seen, size_t alpha_size);
    
public:
    
    Repair(const vector<size_t>& input);
    ~Repair();
    
    string compress(bool verbose = false, bool verbose2 = false);
    TSEQ getSequence() { return this->seq;}
    vector<size_t> getRuleHistory() { return this->ruleHistory;}
    void output(bool verbose = false);
    string encode();
    template<typename T> void serialize(string filename, const T& serialized);
    
};

inline string Repair::compress(bool verbose, bool verbose2)
{
    if(verbose) cout << "[VERBOSE] Running first pass...\n";
    firstPass(verbose2);
    if(verbose) cout << "[VERBOSE] Compressing...\n";
    gen(verbose2);
    if(verbose) cout << "[VERBOSE] Compression ended.\n[VERBOSE] Encoding...\n";
    string enc = encode();
    if(verbose) cout << "[VERBOSE] Done.";
    return enc;
}

inline void Repair::writeuint(size_t s, string& bitString, size_t maxRules)
{
    int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;
    
    for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
    {
        bitString.push_back(((s >> bitPosition) & 1) ? '1' : '0');
    }
}

inline void Repair::encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory,
    string& bitString, size_t maxRules, unordered_set<size_t>& seen, size_t alpha_size)
{
    if (sy >= alpha_size && seen.find(sy) == seen.end())
    {
        seen.insert(sy);
        size_t leftRule  = 2 * (sy - alpha_size);
        size_t rightRule = 2 * (sy - alpha_size) + 1;
        if (rightRule >= ruleHistory.size()) { cerr << "Error\n"; return; }

        bitString.push_back('1');
        writeuint(sy, bitString, maxRules);

        size_t left  = ruleHistory[leftRule];
        size_t right = ruleHistory[rightRule];
        encodeCFG_rec(left,  ruleHistory, bitString, maxRules, seen, alpha_size);
        encodeCFG_rec(right, ruleHistory, bitString, maxRules, seen, alpha_size);
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
    size_t totalRulesNum = ruleHistory.size() / 2;
    size_t maxRules = this->alpha_size + totalRulesNum;
    int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;

    for (int i = 31; i >= 0; --i)
    {
        bitString.push_back(((this->alpha_size >> i) & 1) ? '1' : '0');
    }
    for (int i = 31; i >= 0; --i)
    {
        bitString.push_back(((totalRulesNum >> i) & 1) ? '1' : '0');
    }

    for (size_t i = 0; i < totalRulesNum; i++)
    {
        size_t left  = ruleHistory[2 * i];
        size_t right = ruleHistory[2 * i + 1];
        for (int b = bitsLen - 1; b >= 0; --b)
            bitString.push_back(((left  >> b) & 1) ? '1' : '0');
        for (int b = bitsLen - 1; b >= 0; --b)
            bitString.push_back(((right >> b) & 1) ? '1' : '0');
    }

    vector<size_t> _seq;
    for (const auto& c : seq)
        if (c.code != N) _seq.push_back(c.code);

    for (size_t s : _seq)
    {
        for (int b = bitsLen - 1; b >= 0; --b)
            bitString.push_back(((s >> b) & 1) ? '1' : '0');
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
    static size_t readuint(const string& bitString, size_t& bitPos, size_t maxRules)
    {
        int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;
        size_t value = 0;
        for (int i = 0; i < bitsLen; ++i)
        {
            value <<= 1;
            if (bitString[bitPos++] == '1') value |= 1;
        }
        return value;
    }

    static size_t decodeCFG_rec(const string& bitString, size_t& bitPos,
        vector<Paired>& rules, size_t maxRules, size_t& nextRuleID, size_t alpha_size)
    {
        char flag = bitString[bitPos++];
        if (flag == '1')
        {
            size_t currentRuleID = readuint(bitString, bitPos, maxRules);  // ← read explicit ID
            size_t left  = decodeCFG_rec(bitString, bitPos, rules, maxRules, nextRuleID, alpha_size);
            size_t right = decodeCFG_rec(bitString, bitPos, rules, maxRules, nextRuleID, alpha_size);
            rules[currentRuleID - alpha_size] = {left, right};
            return currentRuleID;
        }
        else
        {
            return readuint(bitString, bitPos, maxRules);
        }
    }

    static void expand(size_t symbol, const vector<Paired>& rules, vector<size_t>& out, size_t alpha_size)
    {
        if (symbol < alpha_size)
        {
            out.push_back(symbol);
        } 
        else
        {
            expand(rules[symbol - alpha_size].left, rules, out, alpha_size);
            expand(rules[symbol - alpha_size].right, rules, out, alpha_size);
        }
    }

public:
static vector<size_t> decompress(const string& bitString)
{
    size_t bitPos = 0;

    auto readBits = [&](size_t maxRules, size_t count) -> size_t
    {
        int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;
        size_t value = 0;
        for (int i = 0; i < bitsLen; ++i)
        {
            value <<= 1; if (bitString[bitPos++] == '1') value |= 1;
        }
        return value;
    };

    size_t alpha_size = 0;
    for (int i = 0; i < 32; i++)
    {
        alpha_size <<= 1; if (bitString[bitPos++] == '1') alpha_size |= 1;
    }
    
    size_t totalRulesNum = 0;
    for (int i = 0; i < 32; i++)
    {
        totalRulesNum <<= 1; if (bitString[bitPos++] == '1') totalRulesNum |= 1;
    }

    size_t maxRules = alpha_size + totalRulesNum;

    vector<Paired> rules(totalRulesNum);
    for (size_t i = 0; i < totalRulesNum; i++)
    {
        rules[i].left  = readBits(maxRules, 0);
        rules[i].right = readBits(maxRules, 0);
    }

    vector<size_t> decoded_sequence;
    while (bitPos < bitString.size())
    {
        size_t symbol = readBits(maxRules, 0);
        expand(symbol, rules, decoded_sequence, alpha_size);
    }
    return decoded_sequence;
}
};


#endif // REPAIR_H
