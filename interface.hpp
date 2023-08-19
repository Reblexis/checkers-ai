#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <inttypes.h>
#include <array>
#include <stack>
#include <string>
#include <vector>

using square = uint_fast8_t;

using move = std::array<uint64_t, 2>;
using bitboard = uint64_t;

class movelist {
public:
	move moves[100];
	move *e;

	movelist();
	move *push();
	move *begin();
	move *end();
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
