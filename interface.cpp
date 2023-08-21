#include "interface.hpp"
#include <math.h>
#include <algorithm>
#include <sstream>

#include <iostream>

#define BBFOR(i, v, s) for (bitboard i = (v); i; i &= i-1) { square s = __builtin_ctz(i);
#define BBFOREND }

uint64_t hash_lookup[32][4];
uint64_t rand64(uint64_t s) {
    s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
    return s * 2685821657736338717LL;
}
void init_hashing() {
	uint64_t rnd = 6819020487127948391ll; // <- seed
	for (size_t i = 0; i < 32; i++) {
		for (size_t j = 0; j < 4; j++) {
			hash_lookup[i][j] = rnd = rand64(rnd);
		}
	}
}

std::string square_vis(square sq) {
	return std::string(1, 
#if BOARD_ORIENTATION_CHESS
		'h' - ((sq & 0x3) << 1 | (~sq >> 2 & 1))
#else
		'h' - ((sq & 0x3) << 1 | (sq >> 2 & 1))
#endif
		) + std::string(1, '1' + (sq >> 2));
}

std::string move_vis(move m) {
	std::ostringstream oss;
	oss << "[" << square_vis(m & 0x1f) << " -> " << square_vis(m >> 5 & 0x1f) << "] ";
	if ((m >> 10) == 0) {
		oss << "walk\n";
	} else {
		oss << "capture of " << __builtin_popcount(m >> 10) << " stones\n";
		//oss << bbvis(m >> 10);
	}
	return oss.str();
}

std::string bbvis(bitboard bb) {
	std::ostringstream oss;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			const bitboard mask = (1 << ((7-i)*4 + 3-j/2));
			oss << (((bb & mask) &&
#if BOARD_ORIENTATION_CHESS
				(i+j)%2
#else
				(i+j)%2 == 0
#endif
				) ? '#' : '.');
		}
		oss << '\n';
	}
	return oss.str();
}

movelist::movelist() : e(moves) { }
movelist::movelist(const movelist &o) : e(moves) {
	for (const move *i = o.cbegin(); i != o.cend(); i++) {
		push(*i);
	}
}
void movelist::push(move m) { *(e++) = m; }
move *movelist::begin() { return moves; }
move *movelist::end() { return e; }
const move *movelist::cbegin() const { return moves; }
const move *movelist::cend() const { return e; }
size_t movelist::size() const { return e-moves; }
bool movelist::contains(move m) const {
	return std::find(cbegin(), cend(), m) != cend();
}

board::board(bitboard w, bitboard b, bool next, bitboard k) : w(w), b(b), wk(w & k), bk(b & k), nextblack(next), hash(0) {
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
template<int8_t dsa, int8_t dsb>
void add_simple(movelist &out, bitboard own) {
	BBFOR(i, own, s)
		out.push(static_cast<uint64_t>(s) | static_cast<uint64_t>(s +
#if BOARD_ORIENTATION_CHESS
			((s >> 2 & 1) ? dsb : dsa)
#else
			((s >> 2 & 1) ? dsa : dsb)
#endif
		) << 5);
	BBFOREND
}
template<bool up, bool down>
void add_jumps(movelist &out, square start, square from, bitboard to_capture, bitboard nall, bitboard captured=0) {
	//std::cout << "add_jumps(movelist " << out.size() << ", start " << square_vis(start) << ", from " << square_vis(from) << ", to_capture:\n" <<
	//	bbvis(to_capture) << ", nall:\n" << bbvis(nall) << ", captured:\n" << bbvis(captured) << ");\n";
	move *ce = out.end();
	bitboard frombb = 1 << from;
	if constexpr (up) {
		const bitboard tlf = tl(frombb);
		if ((to_capture & tlf) && (nall & tl(tlf))) {
			add_jumps<up, down>(out, start, from + 9, to_capture & ~tlf, nall, captured | tlf);
		}
		const bitboard trf = tr(frombb);
		if ((to_capture & trf) && (nall & tr(trf))) {
			add_jumps<up, down>(out, start, from + 7, to_capture & ~trf, nall, captured | trf);
		}
	}
	if constexpr (down) {
		const bitboard blf = bl(frombb);
		if ((to_capture & blf) && (nall & bl(blf))) {
			add_jumps<up, down>(out, start, from - 7, to_capture & ~blf, nall, captured | blf);
		}
		const bitboard brf = br(frombb);
		if ((to_capture & brf) && (nall & br(brf))) {
			add_jumps<up, down>(out, start, from - 9, to_capture & ~brf, nall, captured | brf);
		}
	}
	if (ce == out.end()) {
		//std::cout << "add_jumps - move push (start is " << square_vis(start) << ", end is " << square_vis(from) << ", captured is\n" << bbvis(captured) << ")\n";
		out.push(start | from << 5 | static_cast<move>(captured) << 10);
	}
}
movelist board::moves() const {
	movelist out;
	const bitboard all = b | w;
	const bitboard nall = ~all;
	if (nextblack) {
		// king jumps
		const bitboard upc = (br(w) & br(br(nall))) | (bl(w) & bl(bl(nall)));
		const bitboard jk = bk & ((tr(w) & tr(tr(nall))) | (tl(w) & tl(tl(nall))) | upc);
		BBFOR(i, jk, s)
			add_jumps<true, true>(out, s, s, w, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// normal jumps
		const bitboard j = b & ~bk & upc;
		BBFOR(i, j, s)
			add_jumps<true, false>(out, s, s, w, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;
		
		// walks
		add_simple<5, 4>(out, b & br(nall));
		add_simple<4, 3>(out, b & bl(nall));
		add_simple<-3, -4>(out, bk & tr(nall));
		add_simple<-4, -5>(out, bk & tl(nall));
	} else {
		// king jumps
		const bitboard downc = (tr(b) & tr(tr(nall))) | (tl(b) & tl(tl(nall)));
		const bitboard jk = wk & ((br(b) & br(br(nall))) | (bl(b) & bl(bl(nall))) | downc);
		BBFOR(i, jk, s)
			add_jumps<true, true>(out, s, s, b, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// normal jumps
		const bitboard j = w & ~wk & downc;
		BBFOR(i, j, s)
			add_jumps<false, true>(out, s, s, b, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// walks
		add_simple<-3, -4>(out, w & tr(nall));
		add_simple<-4, -5>(out, w & tl(nall));
		add_simple<5, 4>(out, wk & br(nall));
		add_simple<4, 3>(out, wk & bl(nall));
	}
	return out;
}
void board::play(const move &m) {
	if(m == 0) {
		std::cout << "move "<< "is INVALID\n" << visualize();
		exit(1);
		return;
	}
	history.push({ b | static_cast<uint64_t>(bk) << 32, w | static_cast<uint64_t>(wk) << 32, hash });
	// unpack move
	const square froms = m & 0x1f;
	const bitboard from = 1 << froms;
	const square tos = m >> 5 & 0x1f;
	const bitboard to = 1 << tos;
	const bitboard captm = m >> 10; // captured piece bitmask
	const bitboard icaptm = ~captm;
	// update board state
	if (nextblack) {
		b = (b & ~from) | to;
		hash ^= hash_lookup[froms][1] ^ hash_lookup[tos][1];
		if (bk & from) { // if it's king, update king info as well
			bk = bk & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xf0000000u) { // promotion to king
			bk |= to;
			hash ^= hash_lookup[tos][2];
		}
		BBFOR(i, w & captm, s) // update hash with captures
			hash ^= hash_lookup[s][0];
		BBFOREND
		BBFOR(i, wk & captm, s)
			hash ^= hash_lookup[s][2];
		BBFOREND
		w &= icaptm; // remove captured stones
		wk &= icaptm;
	} else {
		w = w & ~from | to;
		hash ^= hash_lookup[froms][0] ^ hash_lookup[tos][0];
		if (wk & from) { // if it's king, update king info as well
			wk = wk & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xfu) { // promotion to king
			wk |= to;
			hash ^= hash_lookup[tos][2];
		}
		BBFOR(i, b & captm, s) // update hash with captures
			hash ^= hash_lookup[s][1];
		BBFOREND
		BBFOR(i, bk & captm, s)
			hash ^= hash_lookup[s][2];
		BBFOREND
		b &= icaptm; // remove captured stones
		bk &= icaptm;
	}
	// update next player
	nextblack = !nextblack;
	hash ^= hash_lookup[0][3];
}
void board::undo() {
	b = history.top()[0];
	bk = history.top()[0] >> 32;
	w = history.top()[1];
	wk = history.top()[1] >> 32;
	hash = history.top()[2];
	history.pop();
	nextblack = !nextblack;
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
#if BOARD_ORIENTATION_CHESS
			if (((i+j) & 1) == 0)
#else
			if ((i+j) & 1)
#endif
				oss << ' ';
			else if (wk & mask)
				oss << "\u265A";
			else if (w & mask)
				oss << "\u25A0";
			else if (bk & mask)
				oss << "\u2654";
			else if (b & mask)
				oss << "\u25A1";
			else
				oss << " ";
		}
		oss << " | " << (8-i) << "\n  +---+---+---+---+---+---+---+---+\n";
	}
	oss << "    a   b   c   d   e   f   g   h\nnext: " << (nextblack ? "black" : "white") << ", hash: " << hash << "\n";
	return oss.str();
}
