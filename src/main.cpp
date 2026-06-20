#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include "gcis/gcis.h"
#include "repair/repair.h"

using namespace std;

int main(int argc, char* argv[])
{
    string T;
    if (argc < 2)
    {
        ostringstream buffer;
        buffer << cin.rdbuf();
        T = buffer.str();
        cout << "Read " << T.size() << " characters. (" << T.size() / 1e+6 << " MB)\n";
    }
    else
    {
        unsigned long max_chars = stoul(argv[1]);
        T.resize(max_chars);
        cin.read(&T[0], max_chars);
        T.resize(cin.gcount());
    }

    if (T.empty())
    {
        cerr << "Error: Input string is empty.\n";
        return 1;
    }

    cout << "Original Size: " << T.size() << " bytes (" << T.size() * 8 << " bits)\n\n";

    
    vector<size_t> standalone_repair_input;
    standalone_repair_input.reserve(T.size());
    for (char c : T) 
    {
        standalone_repair_input.push_back(static_cast<size_t>(static_cast<unsigned char>(c)));
    }

    auto start_standalone = chrono::high_resolution_clock::now();
    Repair standalone_repair(standalone_repair_input);
    auto standalone_stop = [&standalone_repair]() -> bool { return !standalone_repair.getQueue().empty(); };
    string standalone_res = standalone_repair.compress(standalone_stop, false, false);
    auto end_standalone = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_standalone = end_standalone - start_standalone;
    
    auto start_decompression_st = chrono::high_resolution_clock::now();
    vector<size_t> recovered_standalone_rp = Despair::decompress(standalone_res);
    auto end_decompression_st = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_decompression_standalone = end_decompression_st - start_decompression_st;

    cout << "Compression Finished - Repair\n";
    cout << "- Compression Time: " << time_standalone.count() << "ms\n";
    cout << "- Decompression Time: " << time_decompression_standalone.count() << "ms\n";
    cout << "- Compressed Size: " << standalone_res.size() << " bits\n\n---\n\n";
        
    return 0;

}