
#include <bits/stdc++.h>


std::map<std::string, int> generalhyp
{
    {"evalution_alg", 0},
    {"search_alg", 0}
};

std::map<std::string, int> evalhyp[1]
{
    {
        {"pawn_value", 1},
        {"king_value", 2},
        {"king_dist", 2},
    }
};

std::map<std::string, int> searchhyp[1]
{
    {
        {"max_depth", 6},
    }
};