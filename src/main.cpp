#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "ai/includes/interface.hpp"
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



std::array<int, 5> play_test(int num_games, int asettings[NUM_HYPERPARAMS], int bsettings[NUM_HYPERPARAMS]) {
	// return {win_A_black, win_B_black, win_A_white, win_B_white, draw}
	message("Play test", true);
	board b(0xfff00000, 0xfff);
	int win_A_black = 0;
	int win_B_black = 0;
	int win_A_white = 0;
	int win_B_white = 0;
	int draw = 0;
	int p2counter = 0;

	for(int i = 0; i < num_games; i++){
		// Do first 4 moves randomly
		for(int j = 0; j<4; j++){
			movelist ml = b.moves();
			move randommove = ml.begin()[rand() % ml.size()];
			b.play(randommove);
		}

		std :: array<int, 2> game_res = test_game(b, asettings, bsettings);
		
		draw += (game_res[0] == 0) + (game_res[1] == 0);
		win_A_black += (game_res[0] == 1);
		win_B_black += (game_res[0] == 2);
		win_A_white += (game_res[1] == 1);
		win_B_white += (game_res[1] == 2);

		std::cout << '.' << std::flush;
/*
		if (i % 50 == 49) {
			std::cout << "[" << (win_A_black + win_A_white) << "/" << (win_B_black + win_B_white) << "]\n";
		}*/

		b = board(0xfff00000, 0xfff);
	}

	return {win_A_black, win_B_black, win_A_white, win_B_white, draw};
	/*message("result:", true);
	std::cout << "wins  |  A  |  B  |\n" <<
				 "black |" << std::setw(5) << win_A_black << "|" << std::setw(5) << win_B_black << "|\n" <<
				 "white |" << std::setw(5) << win_A_white << "|" << std::setw(5) << win_B_white << "|\n" <<
				 "draws " << draw << "\n";*/
}

void test_performance(int input_test_hyperparams[NUM_HYPERPARAMS] = nullptr, int num_games = 20)
{
	int test_hyperparams[NUM_HYPERPARAMS];
	for(int i = 0; i < NUM_HYPERPARAMS; i++)
		test_hyperparams[i] = input_test_hyperparams ? input_test_hyperparams[i] : allhyperparams[i];

	int total_wins = 0;
	int total_draws = 0;
	int total_losses = 0;

	for(int i = 0; i < TESTING_SAMPLES_NUM; i++)
	{
		board b(0xfff00000, 0xfff);
		std::array<int, 5> result = play_test(num_games, test_hyperparams, testing_samples[i]);
		message("Result of test game " + std::to_string(i) + ":", true);
		std::cout<<"Total games: "<<num_games<<"\n";
		std::cout<<"Wins: "<<result[0]+result[2]<<"\n";
		std::cout<<"Losses: "<<result[1]+result[3]<<"\n";
		std::cout<<"Draws: "<<result[4]<<"\n";

		total_draws += result[4];
		total_losses += result[1] + result[3];
		total_wins += result[0] + result[2];
	}

	message("Final result:", true);
	std::cout<<"Total games: "<<num_games*TESTING_SAMPLES_NUM<<"\n";
	std::cout<<"Wins: "<<total_wins<<"\n";
	std::cout<<"Losses: "<<total_losses<<"\n";
	std::cout<<"Draws: "<<total_draws<<"\n";
}

void player_versus_bot(int player_color = 0){
	// player color = 0 means player plays black

    launchApp();

	board b(0xfff00000, 0xfff);
	
	for(int i = 0; i<200; i++){
		std::cout<<b.visualize();
		if(i%2==player_color){
			// First cout possible moves and then let player choose the index of the move
			movelist ml = b.moves();
			std::cout << "Possible moves:\n";
			for(int j = 0; j<ml.size(); j++){
				std::cout << j << ": " << move_vis(ml.begin()[j]);
			}

			std::string sel;
			while (sel.empty()) {
				std::cin >> sel;
				if(sel=="undo"){
					b.undo();
					b.undo();
					i--;
					sel.clear();
					continue;
				}
			}
			if (std::find_if_not(sel.begin(), sel.end(), isdigit) == sel.end()) {
				b.play(ml.begin()[std::stoi(sel)]);
			} else {
				const square sq1 = (('h' - sel[0]) / 2) | (sel[1] - '1') << 2;
				const square sq2 = (('h' - sel[2]) / 2) | (sel[3] - '1') << 2;
				move sel = 0;
				for (move m : ml) {
					if ((m & 0x3ff) == (sq1 | sq2 << 5)) {
						sel = m;
					}
				}
				std::cout << "playing " << move_vis(sel);
				if (sel) {
					b.play(sel);
				}
			}
		}
		else{
			auto selected = findmove(b);
			std::cout << "score: " << selected.first << "\nselected: " << move_vis(selected.second);
			b.play(selected.second);
		}
	}

	message("Game over", true);
	std::cout << b.visualize();
}

int main() {
	init_hashing();
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
