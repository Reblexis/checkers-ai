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
#define APP
//#define TOURNAMENT

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
    HyperparametersAgent agent = HyperparametersAgent(AGENTS_PATH/"basic_agent.json", "basic_agent");
    app.launch();
    Game game{};
    app.gameLoop(game, &agent, nullptr);
#endif
#ifdef TOURNAMENT
    Tournament tournament("fight_test", true, 60, 0);
    HyperparametersAgent agent1 = HyperparametersAgent(AGENTS_PATH/"random_agent.json", "random_agent");
    HyperparametersAgent agent2 = HyperparametersAgent(AGENTS_PATH/"basic_agent.json", "basic_agent");
    ExecutableAgent muj = ExecutableAgent(AGENTS_PATH/"basic_agent", "basic_agent");
    ExecutableAgent patrik = ExecutableAgent(AGENTS_PATH/"patrikuv_bot", "patrik");
    HyperparametersAgent agent3 = HyperparametersAgent(AGENTS_PATH/"basic_agent_no_cache.json", "basic_agent_no_cache");
    std::vector<Agent*> agents = {&patrik, &muj};
    tournament.randomMatches(agents, 200);
#endif
}
