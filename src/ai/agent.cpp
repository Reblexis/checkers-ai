#include <string>
#include <utility>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <array>
#include <format>
#include <fcntl.h>

#include "../includes/constants.hpp"
#include "includes/agent.hpp"
#include "includes/evaluation.hpp"
#include "includes/hyperparameters.hpp"
#include "includes/search_algorithms.hpp"

Agent::Agent(std::string id) : id(std::move(id)) {}

void Agent::initialize(long long timeLimit, bool isBlack) {
    this->timeLimit = timeLimit;
    this->isBlack = isBlack;

    pipe(inpipe_fd);
    pipe(outpipe_fd);
    subprocess_pid = fork();

    if (subprocess_pid == 0) {
        setuid(1001);
        runInBackground();
    } else {
        close(inpipe_fd[0]);
        close(outpipe_fd[1]);
    }
}

std::string Agent::serializeGameState(Game &game, const Timer& timer) {
    std::ostringstream input;

    input<< timer.getRemainingTime()<<'\n';
    input<<game.getGameState().board;

    return input.str();
}

void Agent::deserializeGameState(const std::string &input, Game& game, Timer& timer) const{
    std::istringstream inputStream(input);

    long long milliseconds;
    inputStream>>milliseconds;
    timer.reset(milliseconds);

    Board board(getBoardFromStream(inputStream));
    game.addGameState(GameState(board, isBlack));
}
std::string Agent::communicateWithSubprocess(const std::string& input) {
    write(inpipe_fd[1], input.c_str(), input.size());

    std::string output;
    std::array<char, 1024> buffer{};
    while(output.empty()) {
        ssize_t bytes_read = read(outpipe_fd[0], buffer.data(), buffer.size() - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            output = buffer.data();
        }
    }

    return output;
}

std::string Agent::serializeMove(const Move &move) {
    std::ostringstream outputStream;
    outputStream << move.path.size() << " ";
    for (Pos pos: move.path) {
        outputStream << pos.x << " " << pos.y << " ";
    }
    return outputStream.str();
}

std::pair<int, piece_move> Agent::deserializeMove(const std::string &input, Game &game) {
    std::istringstream outputStream(input);
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

std::pair<int, piece_move> Agent::findBestMove(Game &game, const Timer& timer) {
    std::string input = serializeGameState(game, timer);
    std::string output = communicateWithSubprocess(input);
    return deserializeMove(output, game);
}

void HyperparametersAgent::initialize(long long milliseconds, bool isBlack) {
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

HyperparametersAgent::HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id): Agent(std::move(id)), hyperparameters(hyperparametersPath){}

HyperparametersAgent::HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id): Agent(std::move(id)), hyperparameters(std::move(hyperparameters)){}

void HyperparametersAgent::runInBackground(){

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
}

std::pair<int, piece_move> ExecutableAgent::findBestMove(Game &game, const Timer &timer){
    std::string input = serializeGameState(game, timer);
    std::string output = communicateWithSubprocess(input);
    return deserializeMove(output, game);
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

Player::Player(std::string id) : Agent(std::move(id)) {}

void Player::runInBackground() {
    App app = App();
    auto timer = Timer(timeLimit);
    app.launch();
    Game game{};

    std::array<char, 1024> buffer{};

    // Make the input pipe non-blocking
    int flags = fcntl(inpipe_fd[0], F_GETFL, 0);
    fcntl(inpipe_fd[0], F_SETFL, flags | O_NONBLOCK);

    while(!game.isFinished())
    {
        ssize_t bytes_read = read(inpipe_fd[0], buffer.data(), buffer.size() - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';

            // Convert the received string back to simulateGame state
            std::string receivedData(buffer.data());
            deserializeGameState(receivedData, game, timer);

            Move move = app.getMove(game, timer);

            // Now send the best move back to the main process, if necessary
            std::string serializedMove = serializeMove(move);
            write(outpipe_fd[1], serializedMove.c_str(), serializedMove.size());
        }
        app.refresh(game);
    }
}

