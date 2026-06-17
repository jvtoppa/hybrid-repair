
#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include "repair.h"

using namespace std;



void Repair::print() {

  cout << "Hashtable\n";
  printHashTable();
  cout << "\n";
  
  cout << "Queue\n";
  q.print();
  cout << "\n";

  cout << "Sequence\n";
 seq.printConnections();
}


void Repair::insert(PAIR* p, st current_position)
{  
    auto key = make_pair(p->left, p->right);
    auto it = ht.find(key);

    if (it == ht.end()) 
    {
        p->f_pos = current_position;
        seq[current_position].next_occ = N;
        seq[current_position].prev_occ = N;
        ht[key] = p;
    } 
    else 
    {
        PAIR* existing = it->second;
        seq[current_position].next_occ = existing->f_pos;
        seq[current_position].prev_occ = N;
        
        if (existing->f_pos != N)
        {
            seq[existing->f_pos].prev_occ = current_position;
        }
        
        

        existing->f_pos = current_position;

        existing->freq++;
        
        q.removePair(existing);
        q.addPair(existing);
        
        if(existing->freq > q.largest_pair_pos) q.largest_pair_pos = existing->freq;
        delete p;
        
    }
}

void Repair::firstPass(bool verbose) 
{
    st string_size = seq.size();
    if (string_size < 2) return;

    for (st i = 0; i < string_size; i++)
    {
        seq[i].prev = (i == 0) ? N : i - 1;
        seq[i].next = (i == string_size - 1) ? N : i + 1;
        seq[i].next_occ = N;
        seq[i].prev_occ = N;
    }

    for (st i = 0; i < string_size - 1; i++)
    {
        PAIR* new_pair = new PAIR;
        new_pair->left = seq[i].code;
        new_pair->right = seq[i+1].code;
        insert(new_pair, i); 
    }

    if(verbose)
    print();
    
}


void Repair::decrease(st pos) {
    if (pos == N || seq[pos].code == N) return;
    st next_idx = seq[pos].next;
    if (next_idx == N || seq[next_idx].code == N) return;

    auto it = ht.find({seq[pos].code, seq[next_idx].code});
    if (it != ht.end()) {
        PAIR* p = it->second;
        
        q.removePair(p);          
        st p_occ = seq[pos].prev_occ;
        st n_occ = seq[pos].next_occ;
        
        if (p_occ != N) seq[p_occ].next_occ = n_occ;
        else p->f_pos = n_occ;
        
        if (n_occ != N) seq[n_occ].prev_occ = p_occ;
        
        seq[pos].prev_occ = seq[pos].next_occ = N;

        p->freq--;

        if (p->freq >= 2)
        {
            q.addPair(p);         
        }
        else if (p->freq < 1)
        {
            ht.erase(it);
            delete p;
        }
    }
}

void Repair::replace(st position)
{
    st pos_next = seq[position].next;
    if (pos_next == N) return;

    st L = seq[position].prev;
    st R = seq[pos_next].next;

    if (R != N) decrease(pos_next);

    decrease(position);

    if (L != N) decrease(L);

    seq[position].next = R;
    if (R != N) seq[R].prev = position;

    seq[position].code = this->rule;
    
    seq[position].next_occ = N;
    seq[position].prev_occ = N;

    seq[pos_next].code = N;
    seq[pos_next].next = N;
    seq[pos_next].prev = N;
    seq[pos_next].next_occ = N;
    seq[pos_next].prev_occ = N;

    if (L != N)
    {
        insert(new PAIR{ seq[L].code, seq[position].code }, L);
    }
    if (R != N)
    {
        insert(new PAIR{ seq[position].code, seq[R].code }, position);
    }
}

void Repair::compress(bool verbose)
{

  long ctr = 1;
  
    while (!q.empty())
    {
        if(verbose)
        cout << "\nRound " << ctr << "\n";
    
        PAIR* mostFreqPair = q.pop_max();
        if(!mostFreqPair || mostFreqPair->freq < 2) break;

        ht.erase({mostFreqPair->left, mostFreqPair->right});

        this->ruleHistory.push_back(mostFreqPair->left);
        this->ruleHistory.push_back(mostFreqPair->right);

        st curr = mostFreqPair->f_pos;
        this->rule++;

        while (curr != N) {
            
            st next_occ_node = seq[curr].next_occ; 
            
            st r_node = seq[curr].next;
            
            if (r_node != N && seq[curr].code == mostFreqPair->left && seq[r_node].code == mostFreqPair->right) 
            {
                replace(curr);
            }
            
            curr = next_occ_node;
        }

        delete mostFreqPair;
  
    
    this->ruleHistory = ruleHistory;


    ctr++;
    if(verbose)
    print();
    
    }

}

