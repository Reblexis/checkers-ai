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

Agent::Agent(std::string id) : id(std::move(id)) {
    pipe(inpipe_fd);
    pipe(outpipe_fd);

    subprocess_pid = fork();
    if(subprocess_pid == 0)
        setuid(1001);
}


void HyperparametersAgent::initialize(){
    if(subprocess_pid == 0){
        exit(0);
    }
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

HyperparametersAgent::HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id): Agent(std::move(id)), hyperparameters(hyperparametersPath)
{
    initialize();
}

HyperparametersAgent::HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id): Agent(std::move(id)), hyperparameters(std::move(hyperparameters))
{
    initialize();
}

void HyperparametersAgent::newGame() {

}

void HyperparametersAgent::runInBackground() {

}

std::pair<int, piece_move> HyperparametersAgent::findBestMove(Game &game, const Timer& timer) {
    std::pair<int, piece_move> bestMove = searchAlgorithm->findBestMove(game, timer);
    return bestMove;
}

ExecutableAgent::ExecutableAgent(const std::filesystem::path &executablePath, std::string id)
        : Agent(std::move(id)), executablePath(executablePath) {
    if (!std::filesystem::exists(executablePath)) {
        throw std::invalid_argument(std::format("Executable path {} does not exist.", executablePath.string()));
    }

    if (subprocess_pid == 0) {
        runInBackground();
    } else {
        close(inpipe_fd[0]);
        close(outpipe_fd[1]);
    }
}

std::pair<int, piece_move> ExecutableAgent::findBestMove(Game &game, const Timer &timer){
    std::string input = formatInput(game, timer);
    std::string output = communicateWithSubprocess(input);
    return parseOutput(output, game);
}

std::string ExecutableAgent::formatInput(Game &game, const Timer& timer) {
    std::ostringstream input;

    input<< timer.getRemainingTime()<<'\n';
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

std::string ExecutableAgent::communicateWithSubprocess(const std::string& input) {
    write(inpipe_fd[1], input.c_str(), input.size());

    std::string output;
    std::array<char, 256> buffer{};
    ssize_t bytes_read = read(outpipe_fd[0], buffer.data(), buffer.size() - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        output = buffer.data();
    }

    return output;
}

std::pair<int, piece_move> ExecutableAgent::parseOutput(const std::string &output, Game &game) {
    std::istringstream outputStream(output);
    int numberOfPositions;
    Move bestMove;
    bestMove.rotated = game.getGameState().nextBlack;

    outputStream >> numberOfPositions;
    for (int i = 0; i < numberOfPositions; ++i) {
        int col, row;
        outputStream >> col >> row;
        bestMove.path.emplace_back(col, row);
    }

    return {0, bestMove.getPieceMove()};
}

void ExecutableAgent::newGame() {
    
}

void ExecutableAgent::runInBackground() {
    dup2(inpipe_fd[0], STDIN_FILENO);
    dup2(outpipe_fd[1], STDOUT_FILENO);

    close(inpipe_fd[0]);
    close(inpipe_fd[1]);
    close(outpipe_fd[0]);
    close(outpipe_fd[1]);

    execl(executablePath.c_str(), executablePath.c_str(), (char *)NULL);
    exit(1);
}
