#ifndef HYPERPARAMS_HPP
#define HYPERPARAMS_HPP

#include <map>

constexpr int NUM_SQUARES = 32;

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


/*std::map<std::string, int> generalhyp
{
    {"evalution_alg", 0},
    {"search_alg", 0}
};

std::map<std::string, int> evalhyp[2]
{
    {
        {"pawn_value", 1},
        {"king_value", 3},
        {"king_dist", 2},
    },
    {
        {"pawn_value", 1},
        {"king_value", 3}, 
        {"depth_divisor", 2},
    }
};

std::map<std::string, int> searchhyp[1]
{
    {
        {"max_depth", 6},
    }
};*/

inline int allhyperparams[9] = {0, 0, 1, 3, 2, 1, 3, 3, 10};

inline int pawntable[NUM_SQUARES] = {
    15, 15, 15, 15,
    6, 6, 6, 4,
    6, 6, 4, 4,
    6, 8, 4, 2,
    2, 4, 4, 2,
    2, 4, 4, 2,
    4, 4, 6, 6,
    7, 8, 8, 6
};

inline int kingtable[NUM_SQUARES] = {
    0, 2, 2, 0,
    1, 3, 3, 1,
    1, 4, 4, 4,
    1, 4, 4, 1,
    1, 4, 4, 1,
    1, 4, 4, 0,
    0, 2, 2, 0,
    0, 1, 1, 0
};

#endif