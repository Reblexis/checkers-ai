#ifndef CONSOLE_INTERFACE_HPP
#define CONSOLE_INTERFACE_HPP

#include <iostream>

#include "../communication/includes/game.hpp"
#include "includes/agent.hpp"

class ConsoleInterface {
private:
    Agent *agent;
public:
    ConsoleInterface(Agent* agent);
    void run();
};

#endif