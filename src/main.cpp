
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>  
#include "gcis/gcis.h"
#include "repair/repair.h"
#include "../../include/utils.h"
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
    vector<size_t> s;
    for(auto c : T) s.push_back(c);
    Repair compressor(s);
    string compressed = compressor.compress(true, true);
    cout << "Compressed + encoded size: " << compressed.size() << " bits.";

    cout << "\n\n---\n\nChecking correctness: \n";
    bool fail = false;
    vector<size_t> d = Despair::decompress(compressed);
    string decomp = "";
    size_t count = 0;
    for(char c : d)
    {
        if(c != static_cast<char>(s[count]))
        {
            cout << "decompressed: "<<c << "\n\n original: " << s[count] << "\n"; 
            
            fail = true;    
        }
        count++;
    }
    if(!fail)
    {
        cout << "Correct.\n";
    }
    else
    {
        cout << "Incorrect.\n";
    }

}