#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"
#include "../../meta/includes/timer.hpp"
#include "../../app/includes/app.hpp"

const std::filesystem::path AGENTS_PATH = DATA_PATH / "agents/";

class Agent {
protected:
    long long timeLimit;
    bool isBlack;

    virtual void runInBackground() = 0;
    pid_t subprocess_pid;
    int inpipe_fd[2];
    int outpipe_fd[2];

    static std::string serializeGameState(Game &game, const Timer &timer);
    void deserializeGameState(const std::string &input, Game &game, Timer &timer) const;
    std::string communicateWithSubprocess(const std::string& input, const Timer& timer);
    static std::string serializeMove(const Move &move);
    static std::pair<int, piece_move> deserializeMove(const std::string &input, Game &game);
public:
    const std::string id;
    explicit Agent(std::string id);
    void die();
    ~Agent();
    void initialize(long long timeLimit, bool isBlack);
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer);
};

// Contains agent controlled by hyperparameters and local scripts
class HyperparametersAgent: public Agent {
private:
    Hyperparameters hyperparameters;
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    void initialize(long long timeLimit, bool isBlack);
    void runInBackground() override;

public:
    HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id);
    HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
};

// Contains executable path through which moves are received by specified protocol (described in user documentation)
class ExecutableAgent : public Agent {
private:
    const std::filesystem::path executablePath;

    void runInBackground() override;

public:
    ExecutableAgent(const std::filesystem::path &executablePath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
};

class Player: public Agent {
private:
    void runInBackground() override;

public:
    Player(std::string id);
};
#endif // AGENT_HPP
