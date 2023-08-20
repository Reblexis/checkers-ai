#include <stdlib.h>
#include <chrono>
#include <iostream>
#include "interface.hpp"
#include "hyperparams.hpp"
#include "cache.hpp"
#include "brain.hpp"

#define INTERFACE_TEST
//#define INTERFACE_PERFT
#define SEARCH_ALGORITHM_TEST

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
	b.undo();
	std::cout << b.visualize();
}

void interface_perft() {
	board b(0xfff00000, 0xfff);
	for (int d = 1; d < 13; d++) {
		auto start = std::chrono::high_resolution_clock::now();
		uint64_t res = perft(b, d);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "depth " << d << ": " << res << " nodes [took " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms]\n";
	}
}

void search_algorithm_test(){
	board b(0xfff00000, 0xfff);

	while (1){
		movelist ml = b.moves();
		if (!ml.size())
			break;
		for (move m : ml) {
			std::cout << move_vis(m);
		}
		move selected = findmove(b);
		b.play(selected);
		std::cout << "selected: " << move_vis(selected) << b.visualize();
	}
	b.undo();
	std::cout << b.visualize();

}

int main() {
	init_hashing();
#if defined(INTERFACE_TEST)
	interface_test();
#elif defined(INTERFACE_PERFT)
	interface_perft();
#el
#endif
	return 0;
}
