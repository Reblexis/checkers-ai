#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
#ifdef INTERFACE_TEST
	board b(0xaa55aa0000000000ull, 0x55aa55ull);
	std::cout << b.visualize();
	for (move m : b.moves()) {
		std::cout << move_vis(m) << std::endl;
	}
#endif
	return 0;
}
