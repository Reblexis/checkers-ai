#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <nlohmann/json.hpp>
#include <filesystem>

#include "../../includes/constants.hpp"

const std::filesystem::path DEFAULT_STATISTICS_PATH = DATA_PATH / "default_statistics.json";

const std::string RATING_ID = "rating";
const std::string GAMES_ID = "games";
const std::string WINS_ID = "wins";
const std::string DRAWS_ID = "draws";
const std::string LOSSES_ID = "losses";

constexpr int DEFAULT_RATING = 1000;

class Statistics{
private:
    const std::filesystem::path filePath;
    nlohmann::json data;
public:
    Statistics(const std::filesystem::path &path);
    void save() const;
    int getRating() const;
    void addGame(int enemyRating, double result);
};


#endif // STATISTICS_HPP
