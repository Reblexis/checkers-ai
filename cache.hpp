#ifndef CACHE_HPP
#define CACHE_HPP

#include <array>
#include "interface.hpp"

#define MEASURE_CACHE_FILL

constexpr uint16_t unset_depth = 0;
struct cache_entry {
	uint64_t hash;
	move best=0;
	int score;
	uint16_t depth = unset_depth;
};
template<size_t table_size=262144> // table size must be a power of 2!
class cache {
public:
    inline void set(uint64_t hash, uint16_t depth, int score, move best=0) {
		size_t idx = 3 * (hash & (table_size-1));
		cache_entry &sel = table[idx];
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
		sel.best = best;
		sel.score = score;
		sel.depth = depth;
	}
    inline const cache_entry &get(uint64_t hash) const {
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
	inline float fill_rate() const { return filled / static_cast<float>(table_size * 3); }
#endif
private:
	std::array<cache_entry, 3*table_size> table;
	cache_entry null_entry;
#ifdef MEASURE_CACHE_FILL
	size_t filled = 0;
#endif
};

#endif