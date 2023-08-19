#ifndef CACHE_HPP
#define CACHE_HPP

#include <inttypes.h>

using bitboard = uint64_t;

class cache{
public:
    cache();
    ~cache();
    void set(long long key, int value);
    move get(long long key);
    void remove(int key);
    void clear();
    void print();
}

#endif