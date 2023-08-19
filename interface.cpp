#include "interface.hpp"
#include <math.h>
#include <sstream>

#define BBFOR(i, v, s) for (bitboard i = (v); i; i &= i-1) { square s = __builtin_ctzll(i);
#define BBFOREND }

movelist::movelist() : e(moves) { }
move *movelist::push() { return e++; }
move *movelist::begin() { return moves; }
move *movelist::end() { return e; }

constexpr inline bitboard tl(bitboard x) { return x << 9 & 0xf7f7f7f7f7f7f7f7ull; }
constexpr inline bitboard tr(bitboard x) { return x << 7 & 0x7f7f7f7f7f7f7f7full; }
constexpr inline bitboard bl(bitboard x) { return x >> 7 & 0xf7f7f7f7f7f7f7f7ull; }
constexpr inline bitboard br(bitboard x) { return x >> 9 & 0x7f7f7f7f7f7f7f7full; }

board::board(bitboard w, bitboard b, bool next, bitboard k) : w(w), b(b), wk(w & k), bk(b & k), next(next) { }
void add_simple(movelist &out, bitboard own, int dirshift) {
	BBFOR(i, own, s)
		*out.push() = move({ static_cast<uint64_t>(s) << 6 | static_cast<uint64_t>(s + dirshift) << 12, 0 });
	BBFOREND
}
movelist board::moves() const {
	movelist out;
	const bitboard all = b | w;
	const bitboard nall = b | w;
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
	history.push({ b, w, bk, wk });
	const bitboard from = 1ull << ((m[0] >> 6) & 0x3f);
	if (next) {
		if ((m[0] & 0xff) == 0) {
			const bitboard to = 1ull << ((m[0] >> 12) & 0x3f);
			b = b & ~from | to;
			if (bk & from)
				bk = bk & ~from | to;
			else if (to & 0xff00000000000000ull)
				bk |= to;
		} else {
			// TODO: play - jumps
		}
	} else {
		if ((m[0] & 0xff) == 0) {
			const bitboard to = 1ull << ((m[0] >> 12) & 0x3f);
			w = w & ~from | to;
			if (wk & from)
				wk = wk & ~from | to;
			else if (to & 0xffull)
				wk |= to;
		} else {
			// TODO: play - jumps
		}
	}
	next = !next;
}
void board::undo() {
	b = history.top()[0];
	w = history.top()[1];
	bk = history.top()[2];
	wk = history.top()[3];
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
