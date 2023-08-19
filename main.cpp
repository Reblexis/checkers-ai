#include <stdlib.h>
#include <chrono>
#include <iostream>
#include "interface.hpp"

//#define INTERFACE_TEST
#define INTERFACE_PERFT

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

int main() {
	init_hashing();
#if defined(INTERFACE_TEST)
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
#elif defined(INTERFACE_PERFT)
	board b(0xfff00000, 0xfff);
	for (int d = 1; d < 13; d++) {
		auto start = std::chrono::high_resolution_clock::now();
		uint64_t res = perft(b, d);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "depth " << d << ": " << res << " nodes [took " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms]\n";
	}
#endif
	return 0;
}
