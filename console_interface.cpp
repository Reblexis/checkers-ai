#include "console_interface.hpp"
#include <ctype.h>
#include <algorithm>
#include <iostream>
#include "interface.hpp"
#include "brain.hpp"

board b(0xfff00000, 0xfff);
void run_console_bot() {
	std::cerr << "FV bot - running console interface\n";
	std::string name, next;
	std::cin >> name >> next;
	std::string ln;
	std::getline(std::cin, ln);
	std::getline(std::cin, ln);
	bitboard black = 0, white = 0, king = 0;
	for (size_t i = 0; i < 8; i++) {
		std::getline(std::cin, ln);
		for (size_t j = 0; j < 4; j++) {
			bitboard bb = 1 << (j + i*4);
			size_t idx0 = 3 + j * 4;
			char sel = std::max(ln[idx0], ln[idx0+2]);
			if (isupper(sel)) {
				king |= bb;
				sel += ('a' - 'A');
			}
			if (sel == 'b') {
				black |= bb;
			} else if (sel == 'w') {
				white |= bb;
			}
		}
	}
	b = board(black, white, next == "black", king);
	std::cerr << b.visualize();
}
