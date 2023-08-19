#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <inttypes.h>
#include <vector>

using square = uint_fast8_t;

using move = std::vector<square>;
using bitboard = uint64_t;

class movelist {
public:
	move moves[100];
	move *e;

	movelist();
	void push();
	move *begin();
	move *end();
};

class board {
public:
	bool next;
	bitboard w, b;
	bitboard wq, bq;

	movelist moves() const;
	void play(move m);
	void undo(move m);

	int wcount() const;
	int bcount() const;
};

#endif