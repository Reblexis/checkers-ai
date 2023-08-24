#include "includes/interface.hpp"
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
void initializeHashing() {
	uint64_t rnd = 6819020487127948391ll; // <- seed
	for (size_t i = 0; i < 32; i++) {
		for (size_t j = 0; j < 4; j++) {
			hash_lookup[i][j] = rnd = rand64(rnd);
		}
	}
}

std::string visualizeSquare(square sq) {
	return std::string(1,
#if BOARD_ORIENTATION_CHESS
		'h' - ((sq & 0x3) << 1 | (~sq >> 2 & 1))
#else
		'h' - ((sq & 0x3) << 1 | (sq >> 2 & 1))
#endif
		) + std::string(1, '1' + (sq >> 2));
}

struct Coordinates {
    unsigned int x;
    unsigned int y;
};

Coordinates getCoordinates(unsigned int id)
{
    unsigned int x = (id & 3) * 2 + ((id >> 2) & 1);
    unsigned int y = (id >> 2);

    return {x, y}
}

std::pair<Coordinates, Coordinates> getMoveCoordinates(move moveToDecode)
{
    unsigned long idFrom = moveToDecode & 0x1f;
    unsigned long idTo = moveToDecode >> 5 & 0x1f;

    return {getCoordinates(idFrom), getCoordinates(idTo)};
}


std::string visualizeMove(move m) {
	std::ostringstream oss;
	oss << "[" << visualizeSquare(m & 0x1f) << " -> " << visualizeSquare(m >> 5 & 0x1f) << "] ";
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

moveList::moveList() : e(moves) { }
moveList::moveList(const moveList &o) : e(moves) {
	for (const move *i = o.cbegin(); i != o.cend(); i++) {
		push(*i);
	}
}
void moveList::push(move m) { *(e++) = m; }
move *moveList::begin() { return moves; }
move *moveList::end() { return e; }
const move *moveList::cbegin() const { return moves; }
const move *moveList::cend() const { return e; }
size_t moveList::size() const { return e - moves; }
bool moveList::contains(move m) const {
	return std::find(cbegin(), cend(), m) != cend();
}

Board::Board(bitboard w, bitboard b, bool next, bitboard k) : whitePieces(w), blackPieces(b), whiteKings(w & k), blackKings(b & k), nextblack(next), hash(0) {
	BBFOR(i, w, s)
		hash ^= hash_lookup[s][0];
	BBFOREND
	BBFOR(i, b, s)
		hash ^= hash_lookup[s][1];
	BBFOREND
	BBFOR(i, whiteKings | blackKings, s)
		hash ^= hash_lookup[s][2];
	BBFOREND
	if (next)
		hash ^= hash_lookup[0][3];
}
template<int8_t dsa, int8_t dsb>
void add_simple(moveList &out, bitboard own) {
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
void add_jumps(moveList &out, square start, square from, bitboard to_capture, bitboard nall, bitboard captured=0) {
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
		out.push(start | from << 5 | static_cast<move>(captured) << 10);
	}
}
moveList Board::moves() const {
	moveList out;
	const bitboard all = blackPieces | whitePieces;
	const bitboard nall = ~all;
	if (nextblack) {
		// king jumps
		const bitboard upc = (br(whitePieces) & br(br(nall))) | (bl(whitePieces) & bl(bl(nall)));
		const bitboard jk = blackKings & ((tr(whitePieces) & tr(tr(nall))) | (tl(whitePieces) & tl(tl(nall))) | upc);
		BBFOR(i, jk, s)
			add_jumps<true, true>(out, s, s, whitePieces, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// normal jumps
		const bitboard j = blackPieces & ~blackKings & upc;
		BBFOR(i, j, s)
			add_jumps<true, false>(out, s, s, whitePieces, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// walks
		add_simple<5, 4>(out, blackPieces & br(nall));
		add_simple<4, 3>(out, blackPieces & bl(nall));
		add_simple<-3, -4>(out, blackKings & tr(nall));
		add_simple<-4, -5>(out, blackKings & tl(nall));
	} else {
		// king jumps
		const bitboard downc = (tr(blackPieces) & tr(tr(nall))) | (tl(blackPieces) & tl(tl(nall)));
		const bitboard jk = whiteKings & ((br(blackPieces) & br(br(nall))) | (bl(blackPieces) & bl(bl(nall))) | downc);
		BBFOR(i, jk, s)
			add_jumps<true, true>(out, s, s, blackPieces, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// normal jumps
		const bitboard j = whitePieces & ~whiteKings & downc;
		BBFOR(i, j, s)
			add_jumps<false, true>(out, s, s, blackPieces, nall);
		BBFOREND
		if (out.begin() != out.end())
			return out;

		// walks
		add_simple<-3, -4>(out, whitePieces & tr(nall));
		add_simple<-4, -5>(out, whitePieces & tl(nall));
		add_simple<5, 4>(out, whiteKings & br(nall));
		add_simple<4, 3>(out, whiteKings & bl(nall));
	}
	return out;
}
void Board::play(const move &m) {
	if(m == 0) {
		std::cout << "move "<< "is INVALID\n" << visualize();
		exit(1);
		return;
	}
	history.push({blackPieces | static_cast<uint64_t>(blackKings) << 32, whitePieces | static_cast<uint64_t>(whiteKings) << 32, hash });
	// unpack move
	const square froms = m & 0x1f;
	const bitboard from = 1 << froms;
	const square tos = m >> 5 & 0x1f;
	const bitboard to = 1 << tos;
	const bitboard captm = m >> 10; // captured piece bitmask
	const bitboard icaptm = ~captm;
	// update Board state
	if (nextblack) {
        blackPieces = (blackPieces & ~from) | to;
		hash ^= hash_lookup[froms][1] ^ hash_lookup[tos][1];
		if (blackKings & from) { // if it's king, update king info as well
			blackKings = blackKings & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xf0000000u) { // promotion to king
			blackKings |= to;
			hash ^= hash_lookup[tos][2];
		}
		BBFOR(i, whitePieces & captm, s) // update hash with captures
			hash ^= hash_lookup[s][0];
		BBFOREND
		BBFOR(i, whiteKings & captm, s)
			hash ^= hash_lookup[s][2];
		BBFOREND
        whitePieces &= icaptm; // remove captured stones
		whiteKings &= icaptm;
	} else {
        whitePieces = whitePieces & ~from | to;
		hash ^= hash_lookup[froms][0] ^ hash_lookup[tos][0];
		if (whiteKings & from) { // if it's king, update king info as well
			whiteKings = whiteKings & ~from | to;
			hash ^= hash_lookup[froms][2] ^ hash_lookup[tos][2];
		} else if (to & 0xfu) { // promotion to king
			whiteKings |= to;
			hash ^= hash_lookup[tos][2];
		}
		BBFOR(i, blackPieces & captm, s) // update hash with captures
			hash ^= hash_lookup[s][1];
		BBFOREND
		BBFOR(i, blackKings & captm, s)
			hash ^= hash_lookup[s][2];
		BBFOREND
        blackPieces &= icaptm; // remove captured stones
		blackKings &= icaptm;
	}
	// update next player
	nextblack = !nextblack;
	hash ^= hash_lookup[0][3];
}
void Board::undo() {
    blackPieces = history.top()[0];
    blackKings = history.top()[0] >> 32;
    whitePieces = history.top()[1];
    whiteKings = history.top()[1] >> 32;
	hash = history.top()[2];
	history.pop();
	nextblack = !nextblack;
}
int Board::whitePiecesCount() const { return __builtin_popcountll(whitePieces); }
int Board::blackPiecesCount() const { return __builtin_popcountll(blackPieces); }
int Board::whitePawnsCount() const { return __builtin_popcountll(whitePieces & ~whiteKings); }
int Board::blackPawnsCount() const { return __builtin_popcountll(blackPieces & ~blackKings); }
int Board::whiteKingsCount() const { return __builtin_popcountll(whiteKings); }
int Board::blackKingsCount() const { return __builtin_popcountll(blackKings); }
std::string Board::visualize() const {
	std::ostringstream oss;
	oss << "    a   blackPieces   c   d   e   f   g   h\n";
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
			else if (whiteKings & mask)
				oss << "\u265A";
			else if (whitePieces & mask)
				oss << "\u25A0";
			else if (blackKings & mask)
				oss << "\u2654";
			else if (blackPieces & mask)
				oss << "\u25A1";
			else
				oss << " ";
		}
		oss << " | " << (8-i) << "\n  +---+---+---+---+---+---+---+---+\n";
	}
	oss << "    a   blackPieces   c   d   e   f   g   h\nnext: " << (nextblack ? "black" : "white") << ", hash: " << hash << "\n";
	return oss.str();
}
