#include <string>

// Paths
const std::string DATA_PATH = "data/";
const std::string BOTS_PATH = DATA_PATH + "bots/";
const std::string CURRENT_BOT_PATH = BOTS_PATH + "version_0/";
const std::string EXECUTABLE_PATH = CURRENT_BOT_PATH + "bot";
const std::string HYPERPARAMETERS_PATH = CURRENT_BOT_PATH + "hyperparameters.json";

// Game constants
constexpr int NUM_SQUARES = 32;

// Hyperparameter identifiers
const std::string EVALUATION_ALGORITHM_ID = "evaluation_algorithm";
const std::string SEARCH_ALGORITHM_ID = "search_algorithm";
const std::string PAWN_VALUE_ID = "pawn_value";
const std::string KING_VALUE_ID = "king_value";
const std::string DIFF_MULTIPLIER_ID = "diff_multiplier";
const std::string KING_TABLE_ID = "king_table";
const std::string PAWN_TABLE_ID = "pawn_table";
const std::string MAX_DEPTH_ID = "max_depth";
const std::string USE_CACHE_ID = "use_cache";
const std::string OPERATION_LIMIT_ID = "operation_limit";
const std::string DEBUG_BIT_ID = "debug_bit";
