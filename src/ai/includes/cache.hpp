#ifndef CACHE_HPP
#define CACHE_HPP

#include <array>
#include "../../communication/includes/game.hpp"

constexpr uint16_t UNSET_DEPTH = 0;

struct cacheEntry {
	uint64_t hash{};
	piece_move bestMove=0;
    long long score{};
	uint16_t depth = UNSET_DEPTH;
    long long upperBound = INT32_MIN;
    long long lowerBound = INT32_MAX;
};

template<size_t table_size=4194304, size_t bucket_size=3> // table size must be a power of 2!
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
    int filled = 0;
public:
    inline void set(const GameState &gameState, uint16_t depth, long long score, long long upperBound, long long lowerBound, piece_move best) {
        uint64_t hash = getHash(gameState);
		size_t idx = bucket_size * (hash & (table_size-1));
        for(int i = 0; i < bucket_size; i++){
            if(table[idx + i].hash == hash && table[idx + i].depth <= depth && table[idx + i].upperBound <= upperBound && table[idx + i].lowerBound >= lowerBound){
                table[idx + i] = {hash, best, score, depth, upperBound, lowerBound};
                return;
            }
        }

        for(int i = 0; i < bucket_size; i++){
            if(table[idx + i].depth < depth){
                if(table[idx + i].depth == UNSET_DEPTH)
                    filled++;
                table[idx + i] = {hash, best, score, depth, upperBound};
                return;
            }
        }
	}
    inline const cacheEntry &get(const GameState &gameState) const {
        uint64_t hash = getHash(gameState);
		size_t idx = 3 * (hash & (table_size-1));
        for(int i = 0; i < bucket_size; i++){
            if(table[idx + i].hash == hash){
                return table[idx + i];
            }
        }
        return null_entry;
	}
	inline float fillRate() const { return static_cast<float>(filled) / static_cast<float>(table_size * 3); }
private:
	std::array<cacheEntry, bucket_size * table_size> table{};
	cacheEntry null_entry;
#ifdef MEASURE_CACHE_FILL
	size_t filled = 0;
#endif
};

#endif
