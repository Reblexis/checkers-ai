#include <stdlib.h>
#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
	init_hashing();
#ifdef INTERFACE_TEST
	board b(0xfff00000, 0xfff);
	std::cout << b.visualize();
	movelist ml = b.moves();
	for (move m : ml) {
		std::cout << move_vis(m) << std::endl;
	}
	srand(532904124);
	move *selected = ml.begin() + (rand() % ml.size());
	b.play(*selected);
	std::cout << "selected: " << move_vis(*selected) << "\n" << b.visualize();
	b.undo();
	std::cout << b.visualize();
#endif
	return 0;
}
