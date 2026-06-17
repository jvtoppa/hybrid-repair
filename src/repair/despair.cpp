#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <vector>
#include <fstream>
#include "../../include/utils.h"
using namespace std;



struct Paired
{
    size_t left;
    size_t right;
};

class Despair
{

    vector<size_t> read_sequence(bool verbose)
    {
        ifstream file("output/sequence.rp", ios::binary);
        size_t val;
        vector<size_t> read;

        while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
        {
            read.push_back(val);

            if(!verbose)
            continue;

            if(val <= 255)
            cout <<  static_cast<char>(val);
            else
            cout << "[" << val << "]";
        }
        cout << "\n\n";

        return read;
    }

    vector<size_t> read_rulehistory()
    {
        ifstream file("output/rulehistory.rp", ios::binary);
        size_t val;
        vector<size_t> read;
            
        while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
        {
            read.push_back(val);
        }

        return read;
    }

    void expand(size_t symbol, const vector<paired>& rules, vector<size_t>& out) {
        
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
}
