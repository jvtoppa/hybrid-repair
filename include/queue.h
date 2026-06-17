#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include "pair.h"

using namespace std;


class QUEUE
{
private:
    vector<PAIRNODE*> q;
public:
    st largest_pair_pos = 0;
    QUEUE(const vector<size_t>& s);
    ~QUEUE();
    void print() const;
    void addPair(PAIR* p);
    void removePair(PAIR* p);
    PAIR* pop_max();
    size_t size() const
    {
        return q.size();
    }

    size_t empty() const
    {
        return q.empty();
    }

    PAIRNODE* operator[](size_t i) 
    {
        return q[i];
    }

    const PAIRNODE* operator[](size_t i) const 
    {
        return q[i];
    }

};

inline QUEUE::QUEUE(const vector<size_t>& s)
{
    size_t size = static_cast<size_t>(sqrt(s.size())) + 2;
    q.resize(size);
    for (int i = 0; i < q.size(); i++)
    {
        PAIRNODE *p = new PAIRNODE();
        p->next = nullptr;
        p->p = nullptr;
        q[i] = p;
    }
}

inline QUEUE::~QUEUE()
{
 
for (auto &head : q)
{
    PAIRNODE *curr = head;
    while (curr)
    {
        PAIRNODE *temp = curr;
        curr = curr->next;
        delete temp;
    }
}

}

inline void QUEUE::print() const
{
    for (int i = 0; i < q.size(); ++i)
    {
        cout << "Bucket[" << i << "]: ";
        PAIRNODE *curr = q[i]->next;
        while (curr)
        {
            cout << "(" << curr->p->left << curr->p->right << ":" << curr->p->freq << ") ";
            curr = curr->next;
        }
        cout << endl;
    }
}


inline void QUEUE::addPair(PAIR* p)
{
    st pos = p->freq;
    if (pos >= q.size()) pos = q.size() - 1;

    PAIRNODE* head = q[pos];

    PAIRNODE* newNode = new PAIRNODE();
    newNode->p = p;
    
    newNode->next = head->next;
    newNode->prev = head;
    
    if (head->next)
        head->next->prev = newNode;
    
    head->next = newNode;
    p->node = newNode;
}

inline void QUEUE::removePair(PAIR* p)
{
    if (!p || !p->node) return;

    PAIRNODE* node = p->node;

    if (node->prev)
        node->prev->next = node->next;

    if (node->next)
        node->next->prev = node->prev;

    p->node = nullptr;
    delete node;
}

    inline PAIR* QUEUE::pop_max()
    {
        int start = (largest_pair_pos < q.size()) ? largest_pair_pos : (q.size() - 1);
        for (int i = start; i >= 1; i--) 
        {
            if (q[i]->next != nullptr) 
            {
                this->largest_pair_pos = i;
                PAIRNODE* node_to_pop = q[i]->next;
                PAIR* p = node_to_pop->p;

                removePair(p); 

                return p;
            }
        }
        return nullptr; 
    }

#endif // QUEUE_H