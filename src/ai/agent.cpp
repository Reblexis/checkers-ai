#include <string>
#include <utility>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <array>
#include <format>

#include "../includes/constants.hpp"
#include "includes/agent.hpp"
#include "includes/evaluation.hpp"
#include "includes/hyperparameters.hpp"
#include "includes/search_algorithms.hpp"

Agent::Agent(std::string id) : id(std::move(id)) {}

void HyperparametersAgent::initialize(){
    if(hyperparameters.get<int>(EVALUATION_ALGORITHM_ID) == USE_ADVANCED_EVALUATION)
        evaluation = new AdvancedEvaluation(hyperparameters);
    else
        evaluation = new BasicEvaluation(hyperparameters);

    if(hyperparameters.get<int>(SEARCH_ALGORITHM_ID) == USE_MINIMAX)
        searchAlgorithm = new Minimax(hyperparameters, *evaluation);
    else if(hyperparameters.get<int>(SEARCH_ALGORITHM_ID) == USE_ITERATIVE_MINIMAX)
        searchAlgorithm = new IterativeMinimax(hyperparameters, *evaluation);
    else
        searchAlgorithm = new RandomSearch();
}

HyperparametersAgent::HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id): Agent(id), hyperparameters(hyperparametersPath)
{
    initialize();
}

HyperparametersAgent::HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id): Agent(id), hyperparameters(std::move(hyperparameters))
{
    initialize();
}

std::pair<int, piece_move> HyperparametersAgent::findBestMove(Game &game) const {
    std::pair<int, piece_move> bestMove = searchAlgorithm->findBestMove(game);
    return bestMove;
}
ExecutableAgent::ExecutableAgent(const std::filesystem::path &executablePath, std::string id)
        : Agent(id), executablePath(executablePath) {
    if (!std::filesystem::exists(executablePath)) {
        throw std::invalid_argument(std::format("Executable path {} does not exist.", executablePath.string()));
    }
}

std::pair<int, piece_move> ExecutableAgent::findBestMove(Game &game) const {
    std::string inputString = formatInput(game);
    std::string output = runExecutable(inputString);
    return parseOutput(output);
}

std::string ExecutableAgent::formatInput(Game &game) {
    std::ostringstream input;

    input<<game.getGameState();

    return input.str();
}

std::string ExecutableAgent::runExecutable(const std::string &input) const {
    auto tempInputPath = std::filesystem::temp_directory_path() / "agent_input.txt";
    auto tempOutputPath = std::filesystem::temp_directory_path() / "agent_output.txt";

    std::ofstream inputFile(tempInputPath);
    if (!inputFile) {
        throw std::runtime_error("Failed to create input file");
    }
    inputFile << input;
    inputFile.close();

    std::string command = executablePath.string() + " < " + tempInputPath.string() + " > " + tempOutputPath.string();

    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to execute command");
    }

    std::ifstream outputFile(tempOutputPath);
    if (!outputFile) {
        throw std::runtime_error("Failed to open output file");
    }
    std::stringstream outputBuffer;
    outputBuffer << outputFile.rdbuf();
    outputFile.close();

    std::filesystem::remove(tempInputPath);
    std::filesystem::remove(tempOutputPath);

    return outputBuffer.str();
}

std::pair<int, piece_move> ExecutableAgent::parseOutput(const std::string &output) {
    std::istringstream outputStream(output);
    int numberOfPositions;
    Move bestMove;

    outputStream >> numberOfPositions;
    for (int i = 0; i < numberOfPositions; ++i) {
        int col, row;
        outputStream >> col >> row;
        bestMove.path.emplace_back(col, row);
    }

    return {0, bestMove.getPieceMove()};
}