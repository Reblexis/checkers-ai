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
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent = HyperparametersAgent(std::move(hyperparameters), "first_agent");
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef APP
    App app;
    app.launch();
#endif
#ifdef TOURNAMENT
    Tournament tournament("test_tournament", true);
    HyperparametersAgent agent1 = HyperparametersAgent(DEFAULT_HYPERPARAMETERS_PATH, "default2");
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent2 = HyperparametersAgent(std::move(hyperparameters), "default");
    ExecutableAgent agent3 = ExecutableAgent(std::filesystem::path(DATA_PATH/"agents" / "first_agent"), "first_agent");
    std::vector<Agent*> agents = {&agent1, &agent3};
    tournament.randomMatches(agents, 200, 6);
#endif
}
