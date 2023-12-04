#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <filesystem>
#include <string>

// Paths
const std::filesystem::path DATA_PATH = std::filesystem::current_path().parent_path() / "data/";
const std::filesystem::path BOTS_PATH = DATA_PATH / "agents/";
const std::filesystem::path CURRENT_BOT_PATH = BOTS_PATH / "version_0/";
const std::filesystem::path EXECUTABLE_PATH = "bot";
const std::filesystem::path DEFAULT_HYPERPARAMETERS_PATH = DATA_PATH / "default_hyperparameters.json";
const std::filesystem::path HYPERPARAMETERS_PATH = "hyperparameters.json";

// Game constants
constexpr int NUM_SQUARES = 32;

// Hyperparameter identifiers
const std::string EVALUATION_ALGORITHM_ID = "evaluation_algorithm";
constexpr int USE_BASIC_EVALUATION = 0;
constexpr int USE_ADVANCED_EVALUATION = 1;

const std::string SEARCH_ALGORITHM_ID = "search_algorithm";
constexpr int USE_RANDOM = 0;
constexpr int USE_MINIMAX = 1;
constexpr int USE_ITERATIVE_MINIMAX = 2;

const std::string PAWN_VALUE_ID = "pawn_value";
const std::string KING_VALUE_ID = "king_value";
const std::string DIFF_MULTIPLIER_ID = "diff_multiplier";
const std::string KING_TABLE_ID = "king_table";
const std::string PAWN_TABLE_ID = "pawn_table";

const std::string USE_ALPHA_BETA_ID = "use_alpha_beta";
constexpr int USE_ALPHA_BETA = 1;

const std::string MAX_DEPTH_ID = "max_depth";

const std::string USE_CACHE_ID = "use_cache";
constexpr int USE_CACHE = 1;

const std::string OPERATION_LIMIT_ID = "operation_limit";
const std::string DEBUG_BIT_ID = "debug_bit";

#endif