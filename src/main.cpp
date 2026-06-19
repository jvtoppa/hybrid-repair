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

    vector<uint64_t> gcis_metadata;
    vector<uint32_t> gcis_tokens;
    bool stop = false;
    uint32_t lvl = 1;
    uint64_t best_size = -1;
    uint64_t best_level = 0;
    while(true)
    {
        GCIS<uint32_t> gcis_comp(T);
        int current_level = 0;
        auto gcis_stop = [&current_level, &lvl]() mutable
        { 
            if (current_level >= lvl) return false;
            current_level++;
            return true;
        };
        
        
        //Compression block
        auto start_compression = chrono::high_resolution_clock::now();
        gcis_tokens = gcis_comp.compress(gcis_stop, gcis_metadata);
        vector<size_t> repair_input(gcis_tokens.begin(), gcis_tokens.end());
        Repair repair_comp(repair_input);
        auto repair_stop = [&repair_comp]() -> bool { return !repair_comp.getQueue().empty(); };
        string res = repair_comp.compress(repair_stop, false, false);
        auto end_compression = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> time_compression = end_compression - start_compression;
        //
        
        if(gcis_comp.getCFG().size() == (gcis_comp.getAlphabet_size() - 1))
        {
            stop = true;
        }
        
        size_t metadata_size_bits = gcis_metadata.size() * 64;
        size_t total_compressed_bits = res.size() + metadata_size_bits;
        if(total_compressed_bits <= best_size)
        {
            best_size = total_compressed_bits;
            best_level = lvl;
        }
        
        lvl++;    
        
        //Decompression block
        auto start_decompression = chrono::high_resolution_clock::now();
        vector<size_t> recovered_rp = Despair::decompress(res);
        vector<uint32_t> intermediate_gcis_cfg(recovered_rp.begin(), recovered_rp.end());
        vector<uint64_t> final_decoded_output = GCIS<uint32_t>::decompress(gcis_metadata, intermediate_gcis_cfg);
        string reconstructed_string(final_decoded_output.begin(), final_decoded_output.end());
        auto end_decompression = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> time_decompression = end_decompression - start_decompression;
        cout << "Compression Finished - Hybrid (Depth = " << lvl - 1 << ")\n";
        cout << "- Compression time  : "<< time_compression.count() << "ms\n";
        cout << "- Decompression time: "<< time_decompression.count() << "ms\n";
        cout << "- Re-Pair Size      : " << res.size() << " bits\n";
        cout << "- GCIS Size         : " << metadata_size_bits << " bits\n";
        cout << "- Total Size        : " << total_compressed_bits << " bits\n\n";
        //
        /*
        cout << "\nPipeline Check\n";
        
        bool repair_passed = true;
        if (recovered_rp.size() != gcis_tokens.size())
        {
            repair_passed = false;
        }
        else
        {
            for (size_t i = 0; i < gcis_tokens.size(); i++)
            {
                if (gcis_tokens[i] != recovered_rp[i])
                {
                    repair_passed = false;
                    break;
                }
            }
        }
        
        if (repair_passed)
        {
            cout << "[1/2] DESPAIR: SUCCESS\n";
        }
        else
        {
            cout << "[1/2] DESPAIR: FAILURE\n";
        }
        
        vector<uint64_t> gcis_decode = GCIS<uint32_t>::decompress(gcis_metadata, gcis_tokens);
        string gcis_decomp(gcis_decode.begin(), gcis_decode.end());
        
        if (T == gcis_decomp)
        {
            cout << "[2/2] GCIS Decompression: SUCCESS\n";
        }
        else
        {
            cout << "[2/2] GCIS Decompression: FAILURE\n";
        }
        
        if (T == reconstructed_string)
        {
            cout << "PIPELINE: SUCCESS\n";
        }
        else
        {
            cout << "PIPELINE: FAILURE\n";
        }
        */
        if(stop)
        {
            break;
        }
    }
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
    
    cout << "Re-Pair + GCIS Best Size: " << best_size << " bits - level " << best_level << "\n";
    return 0;
}