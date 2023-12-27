#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <filesystem>

#include "../../includes/constants.hpp"

const std::filesystem::path DEFAULT_HYPERPARAMETERS_PATH = DATA_PATH / "default_hyperparameters.json";

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

const std::string MAX_DEPTH_ID = "max_depth";

const std::string USE_CACHE_ID = "use_cache";

const std::string REORDER_MOVES_ID = "reorder_moves";

const std::string MOVE_TIME_LIMIT_ID= "move_time_limit";
const std::string DEBUG_BIT_ID = "debug_bit";

class Hyperparameters
{
private:
    std::string filePath;
    nlohmann::json data;

public:
    explicit Hyperparameters(const std::filesystem::path &path);
    explicit Hyperparameters(const std::string &hyperparameters);

    template<typename T>
    inline void set(const std::string& key, const T& value){
        data[key] = value;
        save();
    };

    template<typename T>
    inline T get(const std::string& key)
    {
        if (data.find(key) == data.end())
        {
            T defaultValue;
            set(key, defaultValue);
        }
        return data[key].get<T>();
    };

    // Functions to save the current data back to the file
    void save() const;
};


