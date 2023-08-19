
#include <bits/stdc++.h>

constexpr int NUM_SQUARES = 32;

std::map<std::string, int> generalhyp
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
};

int pawntable[NUM_SQUARES] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 4, 4, 4,
    4, 4, 4, 0,
    0, 4, 8, 8,
    8, 8, 4, 0,
    0, 4, 8, 16,
    16, 8, 4, 0
};

int kingtable[NUM_SQUARES] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 4, 4, 4,
    4, 4, 4, 0,
    0, 4, 8, 8,
    8, 8, 4, 0,
    0, 4, 8, 16,
    16, 8, 4, 0
};
