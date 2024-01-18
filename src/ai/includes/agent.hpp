#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"
#include "../../meta/includes/timer.hpp"

const std::filesystem::path AGENTS_PATH = DATA_PATH / "agents/";

class Agent {
public:
    const std::string id;
    pid_t subprocess_pid;
    int inpipe_fd[2];
    int outpipe_fd[2];

    explicit Agent(std::string id);
    virtual void newGame() = 0;
    virtual void runInBackground() = 0;
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) = 0; // Pure virtual function
};

// Contains agent controlled by hyperparameters and local scripts
class HyperparametersAgent: public Agent {
private:
    Hyperparameters hyperparameters;
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    void initialize();
    void runInBackground() override;

public:
    HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id);
    HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id);
    void newGame() override;
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
};

// Contains executable path through which moves are received by specified protocol (described in user documentation)
class ExecutableAgent : public Agent {
private:
    const std::filesystem::path executablePath;

    static std::string formatInput(Game &game, const Timer &timer);
    std::string runExecutable(const std::string &input) const;
    static std::pair<int, piece_move> parseOutput(const std::string &output, Game &game);
    void runInBackground() override;
    std::string communicateWithSubprocess(const std::string& input);

public:
    ExecutableAgent(const std::filesystem::path &executablePath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
    void newGame() override;
};

#endif // AGENT_HPP
