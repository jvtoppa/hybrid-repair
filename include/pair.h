#ifndef PAIR_H
#define PAIR_H


#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>


typedef size_t st;
const st N = std::numeric_limits<st>::max();
using namespace std;

struct PAIRNODE;

struct PAIR
{
    st left;
    st right;
    mutable st freq = 1;
    st f_pos = N; // beginning pos
    st b_pos = N; // ending pos
    struct PAIR *next;
    struct PAIRNODE *node = nullptr;
    bool operator==(const PAIR &other) const
    {
        return left == other.left && right == other.right;
    }

    bool operator<(const PAIR &other) const
    {
        return freq < other.freq;
    }
};

struct PAIRNODE
{
    PAIR *p;
    PAIRNODE *next;
    PAIRNODE *prev;
    
};

struct PairHash
{
    size_t operator()(const pair<st, st> &p) const
    {
        return hash<st>()(p.first) ^ (hash<st>()(p.second) << 1);
    }
};


#endif // PAIR_H