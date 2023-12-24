#include <filesystem>

#include "../communication/includes/debugging.hpp"

#include "includes/hyperparameters.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <utility>
#include <format>
#include "../includes/constants.hpp"

Hyperparameters::Hyperparameters(const std::filesystem::path &path): filePath(path)
{
    nlohmann::json defaultData;
    std::ifstream defaultInput(DEFAULT_HYPERPARAMETERS_PATH);
    if (defaultInput.is_open()) {
        defaultInput >> defaultData;
        defaultInput.close();
    }
    else{
        throw std::runtime_error(std::format("Default hyperparameters file {} not found.", DEFAULT_HYPERPARAMETERS_PATH.string()));
    }

    std::ifstream input(filePath);
    if (input.is_open())
    {
        input >> data;
        input.close();
        for (auto& [key, value] : defaultData.items())
        {
            if (data.find(key) == data.end())
            {
                data[key] = value;
                save();
            }
        }
    }
    else{
        data = defaultData;
        save();
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
