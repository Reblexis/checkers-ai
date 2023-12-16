#include "includes/statistics.hpp"

#include <fstream>
#include <iostream>


Statistics::Statistics(const std::filesystem::path &path): filePath(path)
{
    std::filesystem::create_directories(path.parent_path());
    std::ifstream input(filePath);
    if (input.is_open())
    {
        input >> data;
        input.close();
    }
    else
    {
        std::ifstream defaultInput(DEFAULT_STATISTICS_PATH);
        if (defaultInput.is_open())
        {
            defaultInput >> data;
            defaultInput.close();
            save();
        }
    }
}

void Statistics::save() const
{
    std::ofstream output(filePath);
    if (output.is_open())
    {
        output << std::setw(4) << data << std::endl;
        output.close();
    }
    std::cout<<"Saving statistics from "<<filePath<<"\n";
}

int Statistics::getRating() const
{
    return data[RATING_ID].get<int>();
}

void Statistics::addGame(const int enemyRating, double result)
{
    data[GAMES_ID] = data[GAMES_ID].get<int>() + 1;
    if(result >= 0.9)
        data[WINS_ID] = data[WINS_ID].get<int>() + 1;
    else if(result <= 0.1)
        data[LOSSES_ID] = data[LOSSES_ID].get<int>() + 1;
    else
        data[DRAWS_ID] = data[DRAWS_ID].get<int>() + 1;

    // Calculate new rating (https://en.wikipedia.org/wiki/Elo_rating_system)
    double exponent = static_cast<double>(enemyRating - data[RATING_ID].get<int>()) / 400.0;
    double expectedScore = 1.0 / (1.0 + pow(10.0, exponent));
    double newRating = data[RATING_ID].get<int>() + 32.0 * (result - expectedScore);

    data[RATING_ID] = static_cast<int>(round(newRating));

    save();
}

