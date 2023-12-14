#ifndef CONSOLE_INTERFACE_HPP
#define CONSOLE_INTERFACE_HPP

#include <iostream>

#include "game.hpp"
#include "../../ai/includes/agent.hpp"

class ConsoleInterface {
private:
    Agent *agent;
public:
    ConsoleInterface(Agent* agent);
    void run();
};

#endif