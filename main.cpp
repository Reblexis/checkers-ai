#include <stdlib.h>
#include <chrono>
#include <iostream>
#include "interface.hpp"
#include "brain.hpp"
#include "hyperparams.hpp"
#include "cache.hpp"
#include "console_interface.hpp"

//#define INTERFACE_TEST
//#define INTERFACE_PERFT
#define SEARCH_ALGORITHM_TEST

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

	board b(0xfff00000, 0xfff);
	srand(532904124);
	std::cout << b.visualize();
	while (1){
		movelist ml = b.moves();
		if (!ml.size())
			break;
		for (move m : ml) {
			std::cout << move_vis(m);
		}
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

		if(b.nextblack)
			allhyperparams[GH_EVALUATION_ALG] = 1;
		else
			allhyperparams[GH_EVALUATION_ALG] = 1;

		auto selected = findmove(b);
		b.play(selected.second);
		std::cout << "score: " << selected.first << "\nselected: " << move_vis(selected.second) << b.visualize();
		std::cout << "cache fill rate: " << (c.fill_rate() * 100) << "%\n";
	}
	message("Game over", true);
	std::cout << b.visualize();

}

int main() {
	init_hashing();
#if defined(INTERFACE_TEST)
	interface_test();
#elif defined(INTERFACE_PERFT)
	interface_perft();
#elif defined(SEARCH_ALGORITHM_TEST)
	search_algorithm_test();
#else
	run_console_bot();
#endif
	return 0;
}
