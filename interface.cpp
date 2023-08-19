#include "interface.hpp"
#include <math.h>
#include <sstream>

#include <limits>
#include <random>

#define BBFOR(i, v, s) for (bitboard i = (v); i; i &= i-1) { square s = __builtin_ctzll(i);
#define BBFOREND }

uint64_t hash_lookup[32][4];
void init_hashing() {
	std::mt19937_64 rand;
	for (size_t i = 0; i < 32; i++) {
		for (size_t j = 0; j < 4; j++) {
			hash_lookup[i][j] = std::uniform_int_distribution<uint64_t>(0, std::numeric_limits<uint64_t>::max())(rand);
		}
	}
}

std::string square_vis(square sq) {
	return std::string(1, 'h' - (sq & 0x7)) + std::string(1, '1' + (sq >> 3));
}

std::string move_vis(const move &m) {
	std::ostringstream oss;
	if ((m[0] & 0xf) == 0) {
		oss << "walk " << square_vis(m[0] >> 4 & 0x3f) << " -> " << square_vis(m[0] >> 10 & 0x3f);
	} else {
		// TODO: other move types
		oss << "unknown move " << m[0] << " " << m[1];
	}
	return oss.str();
}

std::string bbvis(bitboard bb) {
	std::ostringstream oss;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			const bitboard mask = (1ull << ((7-i)*8 + 7-j));
			oss << (bb & mask ? '#' : '.');
		}
		oss << '\n';
	}
	return oss.str();
}

movelist::movelist() : e(moves) { }
move *movelist::push() { return e++; }
move *movelist::begin() { return moves; }
move *movelist::end() { return e; }
size_t movelist::size() const { return e-moves; }

board::board(bitboard w, bitboard b, bool next, bitboard k) : w(w), b(b), wk(w & k), bk(b & k), next(next), hash(0) {
	BBFOR(i, w, s)
		hash ^= hash_lookup[s >> 1][0];
	BBFOREND
	BBFOR(i, b, s)
		hash ^= hash_lookup[s >> 1][1];
	BBFOREND
	BBFOR(i, wk | bk, s)
		hash ^= hash_lookup[s >> 1][2];
	BBFOREND
	if (next)
		hash ^= hash_lookup[0][3];
}
void add_simple(movelist &out, bitboard own, int dirshift) {
	BBFOR(i, own, s)
		*out.push() = move({ static_cast<uint64_t>(s) << 4 | static_cast<uint64_t>(s + dirshift) << 10, 0 });
	BBFOREND
}
movelist board::moves() const {
	movelist out;
	const bitboard all = b | w;
	const bitboard nall = ~all;
	if (next) {
		/*bitboard l = b & br(w) & ~br(br(all));
		bitboard r = b & bl(w) & ~bl(bl(all));
		BBFOR(i, l, s)
			move *m = out.push();
			*m = move({ s, (square)(s + 18) });
			// TODO: multijump
		BBFOREND
		// TODO: jumps right
		*/
		if (out.begin() == out.end()) {
			add_simple(out, b & br(nall), 9);
			add_simple(out, b & bl(nall), 7);
			add_simple(out, bk & tr(nall), -7);
			add_simple(out, bk & tl(nall), -9);
		}
	} else {
		if (out.begin() == out.end()) {
			add_simple(out, w & tr(nall), -7);
			add_simple(out, w & tl(nall), -9);
			add_simple(out, wk & br(nall), 9);
			add_simple(out, wk & bl(nall), 7);
		}
	}
	return out;
}
void board::play(const move &m) {
	history.push({ b, w, bk, wk, hash });
	const square froms = m[0] >> 4 & 0x3f;
	const bitboard from = 1ull << froms;
	if ((m[0] & 0xf) == 0) {
		const square tos = m[0] >> 10 & 0x3f;
		const bitboard to = 1ull << tos;
		if (next) {
			b = b & ~from | to;
			hash ^= hash_lookup[froms >> 1][1] ^ hash_lookup[tos >> 1][1];
			if (bk & from) {
				bk = bk & ~from | to;
				hash ^= hash_lookup[froms >> 1][2] ^ hash_lookup[tos >> 1][2];
			} else if (to & 0xff00000000000000ull) {
				bk |= to;
				hash ^= hash_lookup[tos >> 1][2];
			}
		} else {
			w = w & ~from | to;
			hash ^= hash_lookup[froms >> 1][0] ^ hash_lookup[tos >> 1][0];
			if (wk & from) {
				wk = wk & ~from | to;
				hash ^= hash_lookup[froms >> 1][2] ^ hash_lookup[tos >> 1][2];
			} else if (to & 0xffull) {
				wk |= to;
				hash ^= hash_lookup[tos >> 1][2];
			}
		}
	} else {
		// TODO: play - jumps
	}
	next = !next;
	hash ^= hash_lookup[0][3];
}
void board::undo() {
	b = history.top()[0];
	w = history.top()[1];
	bk = history.top()[2];
	wk = history.top()[3];
	hash = history.top()[4];
	history.pop();
	next = !next;
}
int board::wcount() const { return __builtin_popcountll(w); }
int board::bcount() const { return __builtin_popcountll(b); }
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
			bitboard mask = (1ull << ((7-i)*8 + 7-j));
			if (bk & mask)
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
	oss << "    a   b   c   d   e   f   g   h\nnext: " << (next ? "black" : "white") << "\n";
	return oss.str();
}
