#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <utility>

class Hyperparameters {
private:
    nlohmann::json data{};
    std::string filePath;

public:
    // Constructor to initialize the hyperparameters object with a file path
    explicit Hyperparameters(std::string path) : filePath(std::move(path)) {
        // Load data from the file, if it exists
        std::ifstream input(filePath);
        if (input.is_open()) {
            input >> data;
            input.close();
        }
    }

    // Template function to set a hyperparameter with a given key and value
    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = value;
    }

    // Template function to get a hyperparameter value for a given key
    template<typename T>
    T get(const std::string& key) const {
        return data[key].get<T>();
    }

    // Save the current hyperparameters to the file
    void save() const {
        std::ofstream output(filePath);
        output << std::setw(4) << data << std::endl;
        output.close();
    }
};
