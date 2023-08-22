#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <inttypes.h>
#include <array>
#include <stack>
#include <string>
#include <vector>

#define BOARD_ORIENTATION_CHESS 1

uint64_t rand64(uint64_t s);
void init_hashing();

using square = uint_fast8_t;
std::string square_vis(square sq);

using move = uint64_t;
std::string move_vis(move m);

using bitboard = uint32_t;
#if BOARD_ORIENTATION_CHESS
constexpr inline bitboard tl(bitboard x) { return (x & 0x00f0f0f0) << 4 | (x & 0x07070707) << 5; }
constexpr inline bitboard tr(bitboard x) { return (x & 0x0f0f0f0f) << 4 | (x & 0x00e0e0e0) << 3; }
constexpr inline bitboard bl(bitboard x) { return (x & 0xf0f0f0f0) >> 4 | (x & 0x07070707) >> 3; }
constexpr inline bitboard br(bitboard x) { return (x & 0x0f0f0f00) >> 4 | (x & 0xe0e0e0e0) >> 5; }
#else
constexpr inline bitboard tl(bitboard x) { return (x & 0x0f0f0f0f) << 4 | (x & 0x00707070) << 5; }
constexpr inline bitboard tr(bitboard x) { return (x & 0x00f0f0f0) << 4 | (x & 0x0e0e0e0e) << 3; }
constexpr inline bitboard bl(bitboard x) { return (x & 0x0f0f0f00) >> 4 | (x & 0x70707070) >> 3; }
constexpr inline bitboard br(bitboard x) { return (x & 0xf0f0f0f0) >> 4 | (x & 0x0e0e0e00) >> 5; }
#endif
std::string bbvis(bitboard bb);

class movelist {
public:
	move moves[100];
	move *e;

	movelist();
	movelist(const movelist &);
	void push(move m);
	move *begin();
	move *end();
	const move *cbegin() const;
	const move *cend() const;
	size_t size() const;
	bool contains(move m) const;
};

class board {
public:
	bool nextblack;
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
