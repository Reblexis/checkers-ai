#include "includes/console_interface.hpp"
#include <ctype.h>
#include <algorithm>
#include <iostream>

#include "includes/game.hpp"
#include "../ai/includes/hyperparameters.hpp"
#include "../ai/includes/search_algorithms.hpp"
#include "../ai/includes/agent.hpp"

template<bool up, bool down>
std::vector<square> get_path(square from, bitboard todo, bitboard free) {
	if (!todo) {
		return std::vector<uint8_t>({from});
	}
	bitboard frombb = 1 << from;
	if constexpr (up) {
		const bitboard tlf = tl(frombb);
		if ((todo & tlf) && (free & tl(tlf))) {
			std::vector<square> res = get_path<up, down>(from + 9, todo & ~tlf, free);
			if (!res.empty()) {
				res.push_back(from);
				return res;
			}
		}
		const bitboard trf = tr(frombb);
		if ((todo & trf) && (free & tr(trf))) {
			std::vector<square> res = get_path<up, down>(from + 7, todo & ~trf, free);
			if (!res.empty()) {
				res.push_back(from);
				return res;
			}
		}
	}
	if constexpr (down) {
		const bitboard blf = bl(frombb);
		if ((todo & blf) && (free & bl(blf))) {
			std::vector<square> res = get_path<up, down>(from - 7, todo & ~blf, free);
			if (!res.empty()) {
				res.push_back(from);
				return res;
			}
		}
		const bitboard brf = br(frombb);
		if ((todo & brf) && (free & br(brf))) {
			std::vector<square> res = get_path<up, down>(from - 9, todo & ~brf, free);
			if (!res.empty()) {
				res.push_back(from);
				return res;
			}
		}
	}
	return {};
}

void print_square_ci(square s) {
	std::cout <<
#if BOARD_ORIENTATION_CHESS
		static_cast<char>('A' + ((s & 0x3) << 1 | (~s >> 2 & 1)))
#else
		static_cast<char>('A' + ((s & 0x3) << 1 | (s >> 2 & 1)))
#endif
			<< static_cast<char>('7' - (s >> 2));
}

Board b(0xfff00000, 0xfff);
void run_console_bot(Agent &agent) {
	//std::cerr << "FV bot - running console interface\n";
	bool bnext;
	{
		std::string next;
		std::cin >> next;
		bnext = next == "black";
		std::getline(std::cin, next); // skip rest of the line
	}
	while (true) {
		std::string ln;
		std::getline(std::cin, ln); // skip jumps
		std::getline(std::cin, ln); // skip letters
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
		b = Board(white, black, bnext, king);
		//std::cerr << blackPieces.visualize();
		auto sm = agent.findBestMove(b);
		move m = sm.second;
		if (m >> 10) {
			std::vector<square> path = get_path<true, true>(m & 0x1f, m >> 10, ~(b.blackPieces | b.whitePieces));
			for (int i = path.size() - 2; i >= 0; i--) {
				print_square_ci(path[i+1]);
				switch (static_cast<int8_t>(path[i+1])-static_cast<int8_t>(path[i])) {
				case 9: std::cout << " tl\n"; break;
				case 7: std::cout << " tr\n"; break;
				case -7: std::cout << " dl\n"; break;
				case -9: std::cout << " dr\n"; break;
				}
				if (i >= 1) {
					for (size_t j = 0; j < 10; j++) { std::getline(std::cin, ln); } // jumps + 9xboard
				}
			}
		} else {
			print_square_ci(m & 0x1f);
			bitboard mfrom = 1 << (m & 0x1f);
			bitboard mto = 1 << (m >> 5 & 0x1f);
			if (mto == tl(mfrom)) {
				std::cout << " dr\n";
			} else if (mto == tr(mfrom)) {
				std::cout << " dl\n";
			} else if (mto == bl(mfrom)) {
				std::cout << " tr\n";
			} else if (mto == br(mfrom)) {
				std::cout << " tl\n";
			}
		}
		//blackPieces.play(m);
		//std::cerr << blackPieces.visualize();
	}
}
