#include <filesystem>

#include "../communication/includes/debugging.hpp"

#include "includes/hyperparameters.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <utility>
#include "../includes/constants.hpp"

Hyperparameters::Hyperparameters(const std::filesystem::path &path): filePath(path)
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
        std::ifstream defaultInput(DEFAULT_HYPERPARAMETERS_PATH);
        if (defaultInput.is_open())
        {
            defaultInput >> data;
            defaultInput.close();
            save();
        }
    }
    message("Hyperparameters loaded from " + filePath);
}

Hyperparameters::Hyperparameters(const std::string &hyperparameters): data(nlohmann::json::parse(hyperparameters)){}

void Hyperparameters::save() const
{
    std::ofstream output(filePath);
    if (output.is_open())
    {
        output << std::setw(4) << data << std::endl;
        output.close();
    }
}
