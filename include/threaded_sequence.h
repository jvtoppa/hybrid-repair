#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "pair.h"
using namespace std;

struct SEQ
{
    st code;
    st prev;
    st next;
    st next_occ;
    st prev_occ;
};

class TSEQ
{
private:
    vector<SEQ> seq;
public:
    TSEQ(const vector<size_t>& s);
    ~TSEQ(){};
    void printConnections() const;
    st next(st pos);
    st prev(st pos);
    SEQ& operator[](st pos)
    {
        return seq[pos];
    }

    const SEQ& operator[](st pos) const
    {
        return seq[pos];
    }

    size_t size() const
    {
        return seq.size();
    }

    bool empty() const
    {
        return seq.empty();
    }

    TSEQ& operator=(vector<SEQ>&& new_vec) {
        this->seq = std::move(new_vec);
        return *this;
    }

    auto begin() const {return seq.begin();}
    auto end() const {return seq.end();}

};

inline TSEQ::TSEQ(const vector<size_t>& s)
{
    st string_size = s.size();
    seq.resize(string_size);
    for (st i = 0; i < string_size; i++)
    {
        seq[i].code = s[i];
    }
}


inline void TSEQ::printConnections() const
{
    for (size_t i = 0; i < seq.size(); i++)
    {
        auto printIndex = [](st idx) -> string
        {
            return (idx == N) ? "NULL" : to_string(idx);
        };

        cout << "SEQ " << i << " (code: " << seq[i].code << ")";

        if (seq[i].code == N)
        {
            cout << " -> Thread: " << printIndex(seq[i].next);
        }
        else
        {
            cout << " -> Next: " << printIndex(seq[i].next);
        }
        if (seq[i].code == N)
        {
            cout << ", Prev. Thread: " << printIndex(seq[i].prev);
        }
        else
        {
            cout << ", Prev: " << printIndex(seq[i].prev);
        }

        cout << ", Next Occ.:" << printIndex(seq[i].next_occ) << ", Prev Occ.:" << printIndex(seq[i].prev_occ)<< "\n";
    }
}

inline st TSEQ::next(st pos) {
    if (pos == N || pos >= seq.size()) return N;
    return seq[pos].next;
}

inline st TSEQ::prev(st pos) {
    if (pos == N || pos >= seq.size()) return N;
    return seq[pos].prev; 
}

#endif // SEQUENCE_H