#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "includes/constants.hpp"

#include "communication/includes/game.hpp"
#include "ai/includes/agent.hpp"
#include "app/includes/app.hpp"
#include "ai/includes/console_interface.hpp"
#include "meta/includes/tournament.hpp"

//#define CLI
#define TOURNAMENT

int main()
{
#ifdef CLI
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent = HyperparametersAgent(std::move(hyperparameters), "first_agent");
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef TOURNAMENT
    Tournament tournament("fight_test", true, 60, 0);
    Player player1("player1");
    Player player2("player2");
    tournament.simulateMatch(&player2, &player1);
#endif
}
