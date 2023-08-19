#include <iostream>
#include "interface.hpp"

#define INTERFACE_TEST

int main() {
#ifdef INTERFACE_TEST
	board b(0xaa55aa0000000000ull, 0x55aa55ull);
	bitboard x = 0xffffffffffffffffull;
	std::cout << b.visualize() << bbvis(x);
	x = tr(x); b.b = tr(b.b); b.w = tr(b.w);
	std::cout << b.visualize() << bbvis(x);
#endif
	return 0;
}
