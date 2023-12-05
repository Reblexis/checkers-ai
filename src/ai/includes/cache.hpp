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
public:
    inline void set(Board &board, uint16_t depth, int score, piece_move best=0) {
		size_t idx = 3 * (board.hash & (table_size-1));
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
		sel.hash = board.hash;
		sel.bestMove = best;
		sel.score = score;
		sel.depth = depth;
	}
    inline const cacheEntry &get(Board &board) const {
		size_t idx = 3 * (board.hash & (table_size-1));
		if (table[idx].hash == board.hash)
			return table[idx];
		if (table[idx+1].hash == board.hash)
			return table[idx+1];
		if (table[idx+2].hash == board.hash)
			return table[idx+2];
		return null_entry;
	}
#ifdef MEASURE_CACHE_FILL
	inline float fillRate() const { return filled / static_cast<float>(table_size * 3); }
#endif
private:
	std::array<cacheEntry, 3 * table_size> table;
	cacheEntry null_entry;
#ifdef MEASURE_CACHE_FILL
	size_t filled = 0;
#endif
};

#endif
