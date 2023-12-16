#ifndef CACHE_HPP
#define CACHE_HPP

#include <array>
#include "../../communication/includes/game.hpp"

#define MEASURE_CACHE_FILL

constexpr uint16_t unset_depth = 0;
struct cacheEntry {
	uint64_t hash{};
	piece_move bestMove=0;
	int score{};
	uint16_t depth = unset_depth;
};

template<size_t table_size=262144> // table size must be a power of 2!
class Cache {
private:
    inline uint64_t rotate_right(uint64_t x, int r) const{
        return (x >> r) | (x << (64 - r));
    }
    inline uint64_t getHash(const GameState &gameState) const {
        const uint64_t c1 = 0xff51afd7ed558ccd;
        const uint64_t c2 = 0xc4ceb9fe1a85ec53;
        Board perspectiveBoard = gameState.getPerspectiveBoard();
        uint64_t k2 = perspectiveBoard.whiteBitboard;
        uint64_t k1 = perspectiveBoard.blackBitboard;

        k1 *= c1; k1 = rotate_right(k1, 31); k1 *= c2;
        k2 *= c2; k2 = rotate_right(k2, 33); k2 *= c1;

        return k1 ^ k2;
    }
public:
    inline void set(const GameState &gameState, uint16_t depth, int score, piece_move best=0) {
        uint64_t hash = getHash(gameState);
		size_t idx = 3 * (hash & (table_size-1));
		cacheEntry &sel = table[idx];
		if (table[idx + 1].depth < sel.depth)
			sel = table[idx + 1];
		if (table[idx + 2].depth < sel.depth)
			sel = table[idx + 2];
#ifdef MEASURE_CACHE_FILL
		if (sel.depth == unset_depth)
			filled++;
		if (depth == unset_depth)
			filled--;
#endif
		sel.hash = hash;
		sel.bestMove = best;
		sel.score = score;
		sel.depth = depth;
	}
    inline const cacheEntry &get(const GameState &gameState) const {
        uint64_t hash = getHash(gameState);
		size_t idx = 3 * (hash & (table_size-1));
		if (table[idx].hash == hash)
			return table[idx];
		if (table[idx+1].hash == hash)
			return table[idx+1];
		if (table[idx+2].hash == hash)
			return table[idx+2];
		return null_entry;
	}
#ifdef MEASURE_CACHE_FILL
	inline float fillRate() const { return static_cast<float>(filled) / static_cast<float>(table_size * 3); }
#endif
private:
	std::array<cacheEntry, 3 * table_size> table{};
	cacheEntry null_entry;
#ifdef MEASURE_CACHE_FILL
	size_t filled = 0;
#endif
};

#endif
