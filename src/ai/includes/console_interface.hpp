#ifndef CONSOLE_INTERFACE_HPP
#define CONSOLE_INTERFACE_HPP

#include <iostream>

#include "../../communication/includes/game.hpp"
#include "agent.hpp"

/**
 * @class ConsoleInterface
 * @brief Agent's console interface.
 *
 * Allows for converting an agent into an executable.
 * Builds an executable agent behaving according to the specified protocol.
 *
 * @var agent The agent to be converted
 */
class ConsoleInterface {
private:
    Agent *agent;
public:
    ConsoleInterface(Agent* agent);
    void run(); ///< Runs the console interface interacting with the agent
};

#endif