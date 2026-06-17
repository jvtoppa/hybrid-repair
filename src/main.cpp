
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>  
#include "gcis.h"
#include <functional>
#include <chrono>
using namespace std;


int main(int argc, char* argv[])
{
    
	string T;
	
	if (argc < 2)
	{
		ostringstream buffer;
		buffer << cin.rdbuf();
		T = buffer.str();
		cout << "Read " << T.size() << " charaters. (" << T.size()  / 1e+6 << " mb)\n";
	}
    else
	{
		unsigned long max_chars = stoul(argv[1]);
			
		T.resize(max_chars);
	
		cin.read(&T[0], max_chars);
		
		T.resize(cin.gcount());

	}
    auto start = chrono::high_resolution_clock::now();    
    vector<uint64_t> c;
    {
    GCIS<uint32_t> compressor(T);
   // T.clear();
    c = compressor.compress
    (
    [&compressor]() -> bool
    {
        if (compressor.getCFG().empty()) 
        {
            return true; 
        }

        return compressor.getCFG().size() != (compressor.getAlphabet_size() - 1); //Get pidgeonholed!
    }
    );
    cout << "\n\n --- \n\n";
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time = end - start;
    cout << "Compression time: "<< time.count() << "ms\n";
    
    }
    
    auto start2 = chrono::high_resolution_clock::now();
    auto decompressed = GCIS<uint32_t>::decompress(c);
    auto end2 = chrono::high_resolution_clock::now();
    
    
    chrono::duration<double, milli> time2 = end2 - start2;
    
    cout << "Decompression time: "<< time2.count() << "ms\n";
    #ifdef EXPERIMENT
    bool fail = false;
    
    for (size_t i = 0; i < T.size(); i++)
    {
        unsigned char orig_byte = static_cast<unsigned char>(T[i]);
        [[unlikely]] if(orig_byte != decompressed[i])
        {fail = true; cout << i << ", T[i] = " << static_cast<int>(orig_byte) << " decompressed[i] = " << decompressed[i] << "\n"; break;}
        
    }
    if(fail)
    {
        cout << "Failed to decompress... Output: ";
        for(char token : decompressed)
        cout << token;
        cout << "\n";
    }
    else
    cout << "[CONSOLE] Decompression worked!\n";
    #endif
   
    cout << "Final size of compression: " <<c.size()*8 / 1e+6 << "mb\n";
}