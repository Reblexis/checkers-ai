#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <filesystem>
#include <string>

// Paths
const std::filesystem::path DATA_PATH = std::filesystem::current_path().parent_path() / "data/";


const std::string CURRENT_AGENT_CONFIG = R"(
{
    "diff_multiplier": 100,
    "evaluation_algorithm": 1,
    "king_table":
    [
        8, 12, 12, 8,
        12, 12, 12, 12,
        15, 15, 15, 12,
        12, 12, 12, 12,
        12, 17, 17, 12,
        12, 18, 18, 15,
        12, 12, 12, 12
    ],
    "king_value": 1,
    "max_depth": 20,
    "pawn_table":
    [
        0, 0, 0, 0,
        10, 10, 10, 10,
        8, 8, 4, 4,
        6, 8, 4, 2,
        2, 4, 4, 2,
        2, 4, 4, 2,
        4, 4, 10, 10,
        7, 9, 10, 10
    ],
    "pawn_value": 1,
    "search_algorithm": 2,
    "move_time_limit": 500,
    "use_alpha_beta": true,
    "use_cache": true,
    "reorder_moves": false
}
)";
#endif