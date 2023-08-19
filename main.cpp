#include <stdlib.h>
#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
	init_hashing();
#ifdef INTERFACE_TEST
	board b(0xaa55aa0000000000ull, 0x55aa55ull);
	std::cout << b.visualize();
	movelist ml = b.moves();
	srand(532904124);
	move *selected = ml.begin() + (rand() % ml.size());
	b.play(*selected);
	std::cout << move_vis(*selected) << "\n" << b.visualize();
	b.undo();
	std::cout << b.visualize();
#endif
	return 0;
}
