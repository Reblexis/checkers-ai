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
constexpr int SH_USE_CACHE = 9;
constexpr int SH_OPERATION_LIMIT = 10;


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

inline int allhyperparams[11] = {1, 0, 1, 3, 2, 1, 3, 10, 999, 1, 10000000};
/*                               e  s  p  k  k  p  k  d   d   c  m
                                 v  e  a  i  i  a  i  i   e   a  a
								 a  a  w  n  n  w  n  f   p   c  x
								 l  r  n  g  g  n  g  f   t   h  O
								 A  c  V  V  D  V  V  m   h   e  p
								 l  g  a  a  i  a  a  u          s
								 g  A  l  l  s  l  l  l
								             t        t
*/

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
    1, 3, 3, 1,
    1, 5, 5, 1,
    1, 6, 6, 1,
    1, 4, 4, 0,
    0, 2, 2, 0,
    0, 1, 1, 0
};

#endif