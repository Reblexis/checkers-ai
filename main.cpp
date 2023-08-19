#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
#ifdef INTERFACE_TEST
	board b(0xaa55aa0000000000ull, 0x55aa55ull);
	std::cout << b.visualize();
#endif
	return 0;
}
