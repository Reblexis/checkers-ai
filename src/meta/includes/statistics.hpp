#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include "nlohmann/json.hpp"
#include <filesystem>

#include "../../includes/constants.hpp"

const std::filesystem::path DEFAULT_STATISTICS_PATH = DATA_PATH / "default_statistics.json"; ///< Path to the default statistics file (contains default statistics setup)

const std::string RATING_ID = "rating";
const std::string GAMES_ID = "games";
const std::string WINS_ID = "wins";
const std::string DRAWS_ID = "draws";
const std::string LOSSES_ID = "losses";

/**
 * @brief Class for managing statistics for the agent.
 *
 * The statistics class is used to manage the statistics for the agent whilst playing tournaments.
 * They are stored in a JSON file and are loaded and saved as needed.
 * The following statistics are stored:
 * - Rating: The current rating of the agent
 * - Games: The number of games played
 * - Wins: The number of games won
 * - Draws: The number of games drawn
 * - Losses: The number of games lost
 *
 * @var filePath The path to the statistics file
 * @var data The JSON data containing the statistics
 */
class Statistics{
private:
    const std::filesystem::path filePath;
    nlohmann::json data;
    void load(); ///< Loads the statistics from the file to local memory
    void save() const; ///< Saves the statistics from local memory to the file
public:
    Statistics(const std::filesystem::path &path);
    int getRating(); ///< Gets the current rating of the agent
    void addGame(int enemyRating, double result); ///< Adds a game to the statistics and adjust the data accordingly
};


#endif // STATISTICS_HPP
