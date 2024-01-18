#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <filesystem>

#include "includes/constants.hpp"

#include "communication/includes/game.hpp"
#include "ai/includes/agent.hpp"
#include "app/includes/app.hpp"
#include "communication/includes/console_interface.hpp"
#include "meta/includes/tournament.hpp"

//#define CLI
#define APP
//#define TOURNAMENT

int main()
{
#ifdef CLI
    Agent agent(CURRENT_AGENT_CONFIG);
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef APP
    std::cout<<"Please provide the path to the first agent (or type 'human'): ";
    std::string agent1Path;
    std::cin>>agent1Path;
    std::optional<Agent> agent1(agent1Path == "human" ? std::nullopt : std::optional<Agent>(Agent(std::filesystem::path(agent1Path))));

    std::cout<<"Please provide the path to the second agent (or type 'human'): ";
    std::string agent2Path;
    std::cin>>agent2Path;
    std::optional<Agent> agent2(agent2Path == "human" ? std::nullopt : std::optional<Agent>(Agent(std::filesystem::path(agent2Path))));

    App app;
    app.launch(agent1, agent2);
#endif
#ifdef TOURNAMENT
    Tournament tournament;
    Agent agent1(AGENTS_PATH / "agent1");
    Agent agent2(AGENTS_PATH / "agent2");
    Agent agent3(AGENTS_PATH / "agent3");
    Agent agent4(AGENTS_PATH / "agent4");
    Agent agent5(AGENTS_PATH / "agent5");
    Agent agent6(AGENTS_PATH / "agent6");
    std::vector<Agent*> agents = {&agent1, &agent2, &agent3, &agent4, &agent5, &agent6};
    tournament.randomMatches(agents, 200, 6);
#endif
}
