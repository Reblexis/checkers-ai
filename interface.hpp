#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <inttypes.h>
#include <array>
#include <stack>
#include <string>
#include <vector>

void init_hashing();

using square = uint_fast8_t;
std::string square_vis(square sq);

using move = uint64_t;
std::string move_vis(move m);

using bitboard = uint32_t;
constexpr inline bitboard tl(bitboard x) { return (x & 0xf0f0f0f) << 4 | (x & 0x707070) << 5; }
constexpr inline bitboard tr(bitboard x) { return (x & 0xf0f0f0f0) << 4 | (x & 0xe0e0e0e) << 5; }
constexpr inline bitboard bl(bitboard x) { return (x & 0xf0f0f00) >> 4 |(x & 0x70707070) >> 3; }
constexpr inline bitboard br(bitboard x) { return (x & 0xf0f0f0f0) >> 4 | (x & 0xe0e0e00) >> 5; }
std::string bbvis(bitboard bb);

class movelist {
public:
	move moves[100];
	move *e;

	movelist();
	void push(move m);
	move *begin();
	move *end();
	size_t size() const;
};

class board {
public:
	bool next;
	bitboard w, b;
	bitboard wk, bk;
	uint64_t hash;
	std::stack<std::array<uint64_t, 3>> history;

	board(bitboard w, bitboard b, bool next=true, bitboard k=0);
	movelist moves() const;
	void play(const move &m);
	void undo();

	int wcount() const;
	int bcount() const;
	int wpcount() const;
	int bpcount() const;
	int wkcount() const;
	int bkcount() const;

	std::string visualize() const;
};

#endif
