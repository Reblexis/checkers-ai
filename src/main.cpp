#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "communication/includes/interface.hpp"
#include "ai/includes/brain.hpp"
#include "ai/includes/hyperparams.hpp"
#include "ai/includes/cache.hpp"

#include "communication/includes/console_interface.hpp"


#include "app/includes/app.hpp"

//#define INTERFACE_TEST
//#define INTERFACE_PERFT
//#define SEARCH_ALGORITHM_TEST
//#define PLAY_TEST
#define PLAYER_VERSUS_BOT

cache<> c;

int main() {
    initializeHashing();
	for(int i = 0; i < NUM_HYPERPARAMS; i++)
		currenthyperparams[i] = allhyperparams[i];

#if defined(INTERFACE_TEST)
	interface_test();
#elif defined(INTERFACE_PERFT)
	interface_perft();
#elif defined(SEARCH_ALGORITHM_TEST)
	search_algorithm_test(allhyperparams, testing_samples[1]);
#elif defined(PLAY_TEST)
	test_performance();
#elif defined(PLAYER_VERSUS_BOT)
	player_versus_bot();
#else
	run_console_bot();
#endif
	return 0;
}
