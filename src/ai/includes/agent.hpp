#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"
#include "../../meta/includes/timer.hpp"
#include "../../app/includes/app.hpp"

const std::filesystem::path AGENTS_PATH = DATA_PATH / "agents/";

/**
 * @class Agent
 * @brief Abstract class for all agents.
 *
 * Agents can be of the following types:
 * - HyperparametersAgent
 * - ExecutableAgent
 * - Player
 *
 * @var timeLimit The time limit for each move
 * @var isBlack Whether the agent is playing as black
 * @var subprocess_pid Process ID of the agent's subprocess
 * @var inpipe_fd Allows for communication with the agent's subprocess (input)
 * @var outpipe_fd Allows for communication with the agent's subprocess (output)
 * @var id The id of the agent (used for logging)
 */
class Agent {
protected:
    long long timeLimit;
    bool isBlack;

    virtual void runInBackground() = 0; ///< Allows the agent to run in the background
    pid_t subprocess_pid=0;
    int inpipe_fd[2];
    int outpipe_fd[2];

    static std::string serializeGameState(Game &game, const Timer &timer); ///< Serializes the game state to a string
    void deserializeGameState(const std::string &input, Game &game, Timer &timer) const; ///< Deserializes the game state from a string
    std::string communicateWithSubprocess(const std::string& input, const Timer& timer, bool receiveOutput = true); ///< Communicates with the agent's subprocess
    static std::string serializeMove(const Move &move); ///< Serializes a move to a string
    static std::pair<int, piece_move> deserializeMove(const std::string &input, Game &game); ///< Deserializes a move from a string
public:
    const std::string id;
    explicit Agent(std::string id);
    void die(); ///< Kills the agent's subprocess
    virtual void initialize(long long timeLimit, bool isBlack); ///< Initializes the agent (a new game is starting)
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer); ///< Returns a chosen move for the given game state

    virtual ~Agent();
    Agent(const Agent&) = delete;
    Agent& operator=(const Agent&) = delete;
    Agent(Agent&&) = delete;
    Agent& operator=(Agent&&) = delete;
};

// Contains agent controlled by hyperparameters and local scripts
/**
 * @class HyperparametersAgent
 * @brief Agent controlled by hyperparameters and local scripts.
 * @var hyperparameters The hyperparameters of the agent
 * @var evaluation The evaluation algorithm used by the agent
 * @var searchAlgorithm The search algorithm used by the agent
 */
class HyperparametersAgent: public Agent {
private:
    Hyperparameters hyperparameters;
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    void initialize(long long timeLimit, bool isBlack) override;
    void runInBackground() override;

public:
    HyperparametersAgent(Hyperparameters &&hyperparameters, std::string id);
    HyperparametersAgent(const std::filesystem::path &hyperparametersPath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
};

// Contains executable path through which moves are received by specified protocol (described in user documentation)
/**
 * @class ExecutableAgent
 * @brief Agent controlled by an executable file.
 *
 * The executable file should be able to receive the game state and return the chosen move according to the specified protocol.
 * @var executablePath The path to the executable
 */
class ExecutableAgent : public Agent {
private:
    const std::filesystem::path executablePath;

    void runInBackground() override;

public:
    ExecutableAgent(const std::filesystem::path &executablePath, std::string id);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer &timer) override;
};

/**
 * @class Player
 * @brief Agent controlled by the player.
 *
 * The player is controlled by the user through the GUI.
 */
class Player: public Agent {
private:
    void runInBackground() override;

public:
    Player(std::string id);
};
#endif // AGENT_HPP
