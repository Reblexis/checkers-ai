#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"
#include "../../meta/includes/timer.hpp"


class Agent {
public:
    const std::string id;

    explicit Agent(std::string id);
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) const = 0; // Pure virtual function
};

// Contains agent controlled by hyperparameters and local scripts
class HyperparametersAgent: public Agent {
private:
    Hyperparameters hyperparameters;
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    void initialize();
public:
    HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id);
    HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) const override;
};

// Contains executable path through which moves are received by specified protocol (described in user documentation)
class ExecutableAgent : public Agent {
private:
    const std::filesystem::path executablePath;
    static std::string formatInput(Game &game, const Timer &timer);
    std::string runExecutable(const std::string &input) const;
    static std::pair<int, piece_move> parseOutput(const std::string &output, Game &game);

public:
    ExecutableAgent(const std::filesystem::path &executablePath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) const override;
};

#endif // AGENT_HPP
