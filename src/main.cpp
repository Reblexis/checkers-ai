#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "includes/constants.hpp"

#include "communication/includes/game.hpp"
#include "ai/includes/agent.hpp"
#include "app/includes/app.hpp"
#include "communication/includes/console_interface.hpp"
#include "meta/includes/tournament.hpp"

//#define CLI
//#define APP
#define TOURNAMENT

int main()
{
#ifdef CLI
    Agent agent(CURRENT_AGENT_CONFIG);
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef APP
    App app;
    app.launch();
#endif
#ifdef TOURNAMENT
    Tournament tournament(true);
    HyperparametersAgent agent1 = HyperparametersAgent(DEFAULT_HYPERPARAMETERS_PATH);
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent2 = HyperparametersAgent(std::move(hyperparameters), "default");
    std::vector<Agent*> agents = {&agent1, &agent2};
    tournament.randomMatches(agents, 200, 6);
#endif
}
