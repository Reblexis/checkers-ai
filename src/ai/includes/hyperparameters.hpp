#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <utility>

class Hyperparameters {
private:
    nlohmann::json data{};
    std::string filePath;

public:
    explicit Hyperparameters(std::string path);

    // Template function to set a hyperparameter with a given key and value
    template<typename T>
    void set(const std::string& key, const T& value);

    // Template function to get a hyperparameter value for a given key
    template<typename T>
    T get(const std::string& key) const;

    // Save the current hyperparameters to the file
    void save() const;
};

