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


#include "app/includes/app.hpp"

//#define INTERFACE_TEST
//#define INTERFACE_PERFT
//#define SEARCH_ALGORITHM_TEST
//#define PLAY_TEST
#define CLI
//#define APP

int main()
{
#ifdef PLAYER_VERSUS_BOT
    playerVersusBot();
#endif
#ifdef APP
    App app;
    app.launch();
#endif
#ifdef CLI
    Agent agent(CURRENT_AGENT_CONFIG);
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
}
