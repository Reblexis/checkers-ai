#include "interface.hpp"
#include <math.h>
#include <sstream>

#include <limits>
#include <random>

#define BBFOR(i, v, s) for (bitboard i = (v); i; i &= i-1) { square s = __builtin_ctz(i);
#define BBFOREND }

uint64_t hash_lookup[32][4];
uint64_t rand64(uint64_t s) {
    s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
    return s * 2685821657736338717LL;
}
void init_hashing() {
	uint64_t rnd = 5819027489127048391ll; // <- seed
	for (size_t i = 0; i < 32; i++) {
		for (size_t j = 0; j < 4; j++) {
			hash_lookup[i][j] = rnd = rand64(rnd);
		}
	}
}

std::string square_vis(square sq) {
	return std::string(1, 'h' - ((sq & 0x3) << 1 | (sq >> 2 & 1))) + std::string(1, '1' + (sq >> 2));
}

std::string move_vis(move m) {
	std::ostringstream oss;
	oss << "[" << square_vis(m >> 4 & 0x1f) << " -> " << square_vis(m >> 9 & 0x1f) << "] ";
	if ((m & 0xf) == 0) {
		oss << "walk";
	} else {
		// TODO: other move types
		oss << "unknown move";
	}
	return oss.str();
}

std::string bbvis(bitboard bb) {
	std::ostringstream oss;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			const bitboard mask = (1 << ((7-i)*4 + 7-j/2));
			oss << (((bb & mask) && (i+j)%2) ? '#' : '.');
		}
		oss << '\n';
	}
	return oss.str();
}

movelist::movelist() : e(moves) { }
void movelist::push(move m) { *(e++) = m; }
move *movelist::begin() { return moves; }
move *movelist::end() { return e; }
size_t movelist::size() const { return e-moves; }

board::board(bitboard w, bitboard b, bool next, bitboard k) : w(w), b(b), wk(w & k), bk(b & k), next(next), hash(0) {
	BBFOR(i, w, s)
		hash ^= hash_lookup[s][0];
	BBFOREND
	BBFOR(i, b, s)
		hash ^= hash_lookup[s][1];
	BBFOREND
	BBFOR(i, wk | bk, s)
		hash ^= hash_lookup[s][2];
	BBFOREND
	if (next)
		hash ^= hash_lookup[0][3];
}
void add_simple(movelist &out, bitboard own, int8_t dsa, int8_t dsb) {
	BBFOR(i, own, s)
		out.push(static_cast<uint64_t>(s) << 4 | static_cast<uint64_t>(s + ((s >> 3 & 1) ? dsb : dsa)) << 9);
	BBFOREND
}
movelist board::moves() const {
	movelist out;
	const bitboard all = b | w;
	const bitboard nall = ~all;
	if (next) {
		const bitboard norm = b & ~bk;
		bitboard l = norm & br(w) & ~br(br(all));
		bitboard r = norm & bl(w) & ~bl(bl(all));
		BBFOR(i, l, s)
		BBFOREND
		if (out.begin() == out.end()) {
			add_simple(out, b & br(nall), 4, 5);
			add_simple(out, b & bl(nall), 3, 4);
			add_simple(out, bk & tr(nall), -4, -3);
			add_simple(out, bk & tl(nall), -5, -4);
		}
	} else {
		if (out.begin() == out.end()) {
			add_simple(out, w & tr(nall), -4, -3);
			add_simple(out, w & tl(nall), -5, -4);
			add_simple(out, wk & br(nall), 4, 5);
			add_simple(out, wk & bl(nall), 3, 4);
		}
	}
	return out;
}
void board::play(const move &m) {
	history.push({ b | static_cast<uint64_t>(bk) << 32, w | static_cast<uint64_t>(wk) << 32, hash });
	const square froms = m >> 4 & 0x1f;
	const bitboard from = 1 << froms;
	const square tos = m >> 9 & 0x1f;
	const bitboard to = 1 << tos;
	if (next) {
		b = (b & ~from) | to;
		hash ^= hash_lookup[froms][1] ^ hash_lookup[tos][1];
		if (bk & from) {
			bk = bk & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xf0000000u) {
			bk |= to;
			hash ^= hash_lookup[tos][2];
		}
	} else {
		w = w & ~from | to;
		hash ^= hash_lookup[froms][0] ^ hash_lookup[tos][0];
		if (wk & from) {
			wk = wk & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xfu) {
			wk |= to;
			hash ^= hash_lookup[tos][2];
		}
	}
	next = !next;
	hash ^= hash_lookup[0][3];
}
void board::undo() {
	b = history.top()[0];
	bk = history.top()[0] >> 32;
	w = history.top()[1];
	wk = history.top()[1] >> 32;
	hash = history.top()[2];
	history.pop();
	next = !next;
}
int board::wcount() const { return __builtin_popcountll(w); }
int board::bcount() const { return __builtin_popcountll(b); }
int board::wpcount() const { return __builtin_popcountll(w & ~wk); }
int board::bpcount() const { return __builtin_popcountll(b & ~bk); }
int board::wkcount() const { return __builtin_popcountll(wk); }
int board::bkcount() const { return __builtin_popcountll(bk); }
std::string board::visualize() const {
	std::ostringstream oss;
	oss << "    a   b   c   d   e   f   g   h\n";
	oss << "  +---+---+---+---+---+---+---+---+\n";
	for (int i = 0; i < 8; i++) {
		oss << (8-i);
		for (int j = 0; j < 8; j++) {
			oss << " | ";
			bitboard mask = (1 << ((7-i)*4 + 3-j/2));
			if ((i+j) & 1)
				oss << ' ';
			else if (bk & mask)
				oss << "\u265A";
			else if (b & mask)
				oss << "\u25A0";
			else if (wk & mask)
				oss << "\u2654";
			else if (w & mask)
				oss << "\u25A1";
			else
				oss << " ";
		}
		oss << " | " << (8-i) << "\n  +---+---+---+---+---+---+---+---+\n";
	}
	oss << "    a   b   c   d   e   f   g   h\nnext: " << (next ? "black" : "white") << ", hash: " << hash << "\n";
	return oss.str();
}
