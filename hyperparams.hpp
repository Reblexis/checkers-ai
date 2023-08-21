#ifndef HYPERPARAMS_HPP
#define HYPERPARAMS_HPP

#include <map>

constexpr int NUM_SQUARES = 32;
constexpr int NUM_HYPERPARAMS = 11;

// THESE ARE ONLY INDICES NOT THE ACTUAL VALUES
constexpr int GH_EVALUATION_ALG = 0;
constexpr int GH_SEARCH_ALG = 1;

constexpr int EH_B_PAWN_VALUE = 2;
constexpr int EH_B_KING_VALUE = 3;
constexpr int EH_B_KING_DIST = 4;

constexpr int EH_A_PAWN_VALUE = 5;
constexpr int EH_A_KING_VALUE = 6;
constexpr int EH_A_DIFF_MULTIPLIER = 7;

constexpr int SH_MAX_DEPTH = 8;
constexpr int SH_USE_CACHE = 9;
constexpr int SH_OPERATION_LIMIT = 10;

inline const int allhyperparams[NUM_HYPERPARAMS] = {1, 0, 1, 3, 2, 1, 3, 10, 999, 0, 10000000};
/*                               e  s  p  k  k  p  k  d   d   c  m
                                 v  e  a  i  i  a  i  i   e   a  a
								 a  a  w  n  n  w  n  f   p   c  x
								 l  r  n  g  g  n  g  f   t   h  O
								 A  c  V  V  D  V  V  m   h   e  p
								 l  g  a  a  i  a  a  u          s
								 g  A  l  l  s  l  l  l
								             t        t
*/
inline int currenthyperparams[NUM_HYPERPARAMS]; // Hyperparams used at the moment by the engine, can change during runtime
// Samples used for performance testing

constexpr int TESTING_SAMPLES_NUM = 6;
constexpr int NUM_OPS = 10000;
constexpr int USE_CACHE = 0;

inline int testing_samples[TESTING_SAMPLES_NUM][NUM_HYPERPARAMS] = {
    // Random 
    {0, 1, 1, 3, 2, 1, 3, 10, 5, USE_CACHE, NUM_OPS},
    // Basic minimax with basic eval
    {0, 2, 1, 3, 2, 1, 3, 10, 5, USE_CACHE, NUM_OPS},
    // Iterative minimax with basic eval
    {0, 0, 1, 3, 2, 1, 3, 10, 999, USE_CACHE, NUM_OPS},
    // Basic minimax with advanced eval
    {1, 2, 1, 3, 2, 1, 3, 10,  5, USE_CACHE, NUM_OPS},
    // Iterative minimax with advanced eval
    {1, 0, 1, 3, 2, 1, 3, 10, 999, USE_CACHE, NUM_OPS},    
    // Iterative minimax with advanced eval and cache
    {1, 0, 1, 3, 2, 1, 3, 10, 999, 0, NUM_OPS},
};



inline int pawntable[NUM_SQUARES] = {
    15, 15, 15, 15,
    8, 8, 6, 4,
    8, 8, 4, 4,
    6, 8, 4, 2,
    2, 4, 4, 2,
    2, 4, 4, 2,
    4, 4, 10, 10,
    7, 9, 10, 10
};

inline int kingtable[NUM_SQUARES] = {
    2, 3, 3, 2,
    4, 4, 4, 4,
    4, 5, 5, 4,
    4, 6, 6, 4,
    4, 6, 6, 4,
    5, 6, 6, 5,
    6, 6, 6, 6
};


#endif