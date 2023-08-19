#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <inttypes.h>
#include <array>
#include <stack>
#include <string>
#include <vector>

using square = uint_fast8_t;
std::string square_vis(square sq);

using move = std::array<uint64_t, 2>;
std::string move_vis(const move &m);

using bitboard = uint64_t;
constexpr inline bitboard tl(bitboard x) { return x << 9 & 0xfefefefefefefefeull; }
constexpr inline bitboard tr(bitboard x) { return x << 7 & 0x7f7f7f7f7f7f7f7full; }
constexpr inline bitboard bl(bitboard x) { return x >> 7 & 0xfefefefefefefefeull; }
constexpr inline bitboard br(bitboard x) { return x >> 9 & 0x7f7f7f7f7f7f7f7full; }
std::string bbvis(bitboard bb);

class movelist {
public:
	move moves[100];
	move *e;

	movelist();
	move *push();
	move *begin();
	move *end();
	size_t size() const;
};

class board {
public:
	bool next;
	bitboard w, b;
	bitboard wk, bk;
	std::stack<std::array<bitboard, 4>> history;

	board(bitboard w, bitboard b, bool next=true, bitboard k=0);
	movelist moves() const;
	void play(const move &m);
	void undo();

	int wcount() const;
	int bcount() const;
	int wkcount() const;
	int bkcount() const;

	std::string visualize() const;
};

#endif
