#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <filesystem>

// Forward declarations (if necessary)
class Board;

class Hyperparameters
{
private:
    nlohmann::json data;
    std::string filePath;

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


