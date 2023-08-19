#include <stdlib.h>
#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
	init_hashing();
#ifdef INTERFACE_TEST
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
#endif
	return 0;
}
