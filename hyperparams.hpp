#ifndef HYPERPARAMS_HPP
#define HYPERPARAMS_HPP

#include <map>

constexpr int NUM_SQUARES = 32;
constexpr int NUM_HYPERPARAMS = 9;

// THESE ARE ONLY INDICES NOT THE ACTUAL VALUES
constexpr int GH_EVALUATION_ALG = 0;
constexpr int GH_SEARCH_ALG = 1;

constexpr int EH_PAWN_VALUE = 2;
constexpr int EH_KING_VALUE = 3;
constexpr int EH_KING_DIST = 4;

constexpr int EH_A_DIFF_MULTIPLIER = 5;

constexpr int SH_MAX_DEPTH = 6;
constexpr int SH_USE_CACHE = 7;
constexpr int SH_OPERATION_LIMIT = 8;

inline const int allhyperparams[NUM_HYPERPARAMS] = 
{
    1, // Evaluation algorithm (0 - basic, 1 - advanced)
    0, // Search algorithm (0 - iterative minimax, 1 - random, 2 - basic minimax)
    1, // Pawn value for evaluation algorithm
    3, // King value for evaluation algorithm
    2, // King distance weight for evaluation algorithm
    10, // Difference multiplier for pieces for evaluation algorithm
    999, // Max depth for search algorithm
    0, // Use cache for search algorithm (0 - no, 1 - yes)
    10000, // Operation limit for search algorithm
};

inline int currenthyperparams[NUM_HYPERPARAMS]; // Hyperparams used at the moment by the engine, can change during runtime
// Samples used for performance testing

constexpr int TESTING_SAMPLES_NUM = 6;
constexpr int NUM_OPS = 10000;
constexpr int USE_CACHE = 0;

inline int testing_samples[TESTING_SAMPLES_NUM][NUM_HYPERPARAMS] = {
    // Random 
    {0, 1, 1, 3, 2, 10, 5, USE_CACHE, NUM_OPS},
    // Basic minimax with basic eval
    {0, 2, 1, 3, 2, 10, 5, USE_CACHE, NUM_OPS},
    // Iterative minimax with basic eval
    {0, 0, 1, 3, 2, 10, 999, USE_CACHE, NUM_OPS},
    // Basic minimax with advanced eval
    {1, 2, 1, 3, 2, 10,  5, USE_CACHE, NUM_OPS},
    // Iterative minimax with advanced eval
    {1, 0, 1, 3, 2, 10, 999, USE_CACHE, NUM_OPS},    
    // Iterative minimax with advanced eval and cache
    {1, 0, 1, 3, 2, 10, 999, 0, NUM_OPS},
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