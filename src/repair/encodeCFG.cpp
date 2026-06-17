#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <unordered_set>
#include <chrono>

using namespace std;

namespace encode
{
    void writeuint(size_t s, string& bitString, size_t maxRules)
    {
        int bitsLen = (maxRules <= 1) ? 1 : static_cast<int>(floor(log2(maxRules))) + 1;
        
        for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
        {
            bitString.push_back(((s >> bitPosition) & 1) ? '1' : '0');
        }
    }

    void encodeCFG_rec(size_t sy, const vector<size_t>& ruleHistory, string& bitString, size_t maxRules, unordered_set<size_t>& seen)
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

    string encode_sequence(const vector<size_t>& seq, const vector<size_t>& ruleHistory)
    {
        string bitString;
        unordered_set<size_t> seen;
        
        size_t maxRules = 256 + (ruleHistory.size() / 2); 

        for (size_t s : seq)
        {
            encodeCFG_rec(s, ruleHistory, bitString, maxRules, seen);
        }
        return bitString;
    }

    void writeToDisk(const string& bitString, const string& outputPath)
    {
        uint32_t bits = static_cast<uint32_t>(bitString.size());
        ofstream out(outputPath, ios::binary);
        unsigned char buffer = 0;
        int bitCount = 0;

        out.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
        for (char bit : bitString) 
        {
            buffer <<= 1;
            if (bit == '1') buffer |= 1;
            bitCount++;

            if (bitCount == 8) {
                out.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }

        if (bitCount > 0)
        {
            buffer <<= (8 - bitCount);
            out.put(buffer);
        }
        out.close();
    }
}