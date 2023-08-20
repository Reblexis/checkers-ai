#include <stdlib.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include "interface.hpp"
#include "brain.hpp"
#include "hyperparams.hpp"
#include "cache.hpp"
#include "console_interface.hpp"

//#define INTERFACE_TEST
//#define INTERFACE_PERFT
#define SEARCH_ALGORITHM_TEST
// #define PLAY_TEST

void message(std::string message, bool important = false){
	if(important)
		std::cout << "--------------------------\n";
	std::cout << message << "\n";
	if(important)
		std::cout << "--------------------------\n";
}


uint64_t perft(board &b, int depth) {
	if (depth < 1)
		return 1;
	movelist ml = b.moves();
	if (depth < 2)
		return ml.size();
	uint64_t counter = 1;
	for (move m : ml) {
		b.play(m);
		counter += perft(b, depth-1);
		b.undo();
	}
	return counter;
}

void interface_test() {
	//bitboard x = 0xffffffff;
	//std::cout << bbvis(x) << "\n" << bbvis(tl(x)) << "\n" << bbvis(tr(x)) << "\n" << bbvis(bl(x)) << "\n" << bbvis(br(x));

	message("Running interface test", true);

	board b(3430946816, 524288, true, 2048);
	srand(532904124);
	std::cout << b.visualize();
	while (1){
		movelist ml = b.moves();
		for (move m : ml) {
			std::cout << move_vis(m);
		}
		if (!ml.size())
			break;
		move *selected = ml.begin() + (rand() % ml.size());
		b.play(*selected);
		std::cout << "selected: " << move_vis(*selected) << b.visualize();
	}
}

void interface_perft() {
	board b(0xfff00000, 0xfff);
	message("Running interface perft test", true);
	for (int d = 1; d < 13; d++) {
		auto start = std::chrono::high_resolution_clock::now();
		uint64_t res = perft(b, d);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "depth " << d << ": " << res << " nodes [took " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms]\n";
	}
}

cache<> c;
void search_algorithm_test(){
	board b(0xfff00000, 0xfff);
	
	message("Running search algorithm test", true);
	std::cout << b.visualize();
	
	int num_moves = 1000;

	while(1){
		if (!num_moves--){
			message("Moves limit reached", true);
			break;
		}
		movelist ml = b.moves();
		if (!ml.size())
			break;
		/*message("Possible moves:", true);
		for (move m : ml) {
			std::cout << move_vis(m);
		}*/

		auto selected = findmove(b);
		std::cout << "score: " << selected.first << "\nselected: " << move_vis(selected.second);
		b.play(selected.second);
		std::cout << b.visualize();
		std::cout << "cache fill rate: " << (c.fill_rate() * 100) << "%\n";
	}
	message("Game over", true);
	std::cout << b.visualize();

}
int test_game(const board &original_board) {
	allhyperparams[SH_MAX_DEPTH] = 7;
	allhyperparams[SH_OPERATION_LIMIT] = 500000;
	allhyperparams[SH_USE_CACHE] = 0;
	board b(original_board);
	movelist ml = b.moves();
	int res = 0;
	for (int gameid = 0; gameid < 2;gameid++) {
		int limit = 0;
		while (ml.size()) {
			if (limit++ > 3000) {
				res |= 1 << gameid << 2;
				break;
			}
			if (b.nextblack ^ (gameid & 1)) {
				// A hyperparam overrides
				allhyperparams[GH_SEARCH_ALG] = 0;
			} else {
				// B hyperparam overrides
				allhyperparams[GH_SEARCH_ALG] = 1;
			}
			move m = findmove(b).second;
			if (!m)
				break;
			b.play(m);
			ml = b.moves();
		}
		res |= b.nextblack << gameid;
	}
	return res;
}
void play_test() {
	message("Play test", true);
	board b(0xfff00000, 0xfff);
	int win_A_black = 0;
	int win_B_black = 0;
	int win_A_white = 0;
	int win_B_white = 0;
	int draw = 0;
	int p2counter = 0;
	for (move m : b.moves()) {
		b.play(m);
		for (move m : b.moves()) {
			b.play(m);
			for (move m : b.moves()) {
				b.play(m);
				for (move m : b.moves()) {
					b.play(m);
					int win = test_game(b);
					if (!(win & 0b100))
						((win & 1) ? win_B_white : win_A_black)++;
					else
					 	draw++;
					if (!(win & 0b1000))
						((win >> 1 & 1) ? win_A_white : win_B_black)++;
					else
						draw++;
					std::cout << '.' << std::flush;
					b.undo();
				}
				b.undo();
			}
			std::cout << " " << (++p2counter) << ". 2-ply state search done - [" << (win_A_black + win_A_white) << "/" << (win_B_black + win_B_white) << "]\n";
			b.undo();
		}
		b.undo();
	}
	message("result:", true);
	std::cout << "wins  |  A  |  B  |\n" <<
				 "black |" << std::setw(5) << win_A_black << "|" << std::setw(5) << win_B_black << "|\n" <<
				 "white |" << std::setw(5) << win_A_white << "|" << std::setw(5) << win_B_white << "|\n" <<
				 "draws " << draw << "\n";
}

int main() {
	init_hashing();
#if defined(INTERFACE_TEST)
	interface_test();
#elif defined(INTERFACE_PERFT)
	interface_perft();
#elif defined(SEARCH_ALGORITHM_TEST)
	search_algorithm_test();
#elif defined(PLAY_TEST)
	play_test();
#else
	run_console_bot();
#endif
	return 0;
}
