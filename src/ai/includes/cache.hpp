#ifndef CACHE_HPP
#define CACHE_HPP

#include <array>
#include "../../communication/includes/game.hpp"

constexpr uint16_t UNSET_DEPTH = 0;

/**
 * @struct CacheEntry
 * @brief Represents an entry in the cache.
 *
 * @var board The board ID used as the key (verification)
 * @var bestMove The best move
 * @var score The score of the board
 * @var depth The depth of the search
 * @var upperBound The upper bound of the score
 * @var lowerBound The lower bound of the score
 */
struct CacheEntry {
    board_id board;
	piece_move bestMove=0;
    long long score{};
	uint16_t depth = UNSET_DEPTH;
    long long upperBound = INT32_MIN;
    long long lowerBound = INT32_MAX;
};


/**
 * @class Cache
 * @brief Manages a cache for storing best moves and scores.
 *
 * A cache that stores the best moves and scores for a given board state. The cache is implemented as a hash table with buckets.
 * It also attempts to be work efficiently with alpha beta pruning.
 *
 * @tparam table_size The size of the cache
 * @tparam bucket_size The size of each bucket
 */
template<size_t table_size=4194304, size_t bucket_size=3> // table size must be a power of 2!
class Cache {
private:
    inline uint64_t rotate_right(uint64_t x, int r) const{
        return (x >> r) | (x << (64 - r));
    }
    inline uint64_t getHash(const Board board) const {
        const uint64_t c1 = 0xff51afd7ed558ccd;
        const uint64_t c2 = 0xc4ceb9fe1a85ec53;
        uint64_t k2 = board.whiteBitboard;
        uint64_t k1 = board.blackBitboard;

        k1 *= c1; k1 = rotate_right(k1, 31); k1 *= c2;
        k2 *= c2; k2 = rotate_right(k2, 33); k2 *= c1;

        return k1 ^ k2;
    }
    int filled = 0;
public:
    /**
     * @brief Attempts to save the best move and score for a given board state
     * @param gameState The game state
     * @param depth The depth of the search
     * @param score The found score for the board
     * @param upperBound The upper bound of the score
     * @param lowerBound The lower bound of the score
     * @param best The found best move
     */
    inline void set(const GameState &gameState, uint16_t depth, long long score, long long upperBound, long long lowerBound, piece_move best) {
        Board perspectiveBoard = gameState.getPerspectiveBoard(); // Get the board from the perspective of the current player optimal moves should be the same for both sides with correct evaluation
        board_id boardID = perspectiveBoard.getID();
        uint64_t hash = getHash(perspectiveBoard);
		size_t idx = bucket_size * (hash & (table_size-1)); // Hash % table_size only for power of 2
        for(int i = 0; i < bucket_size; i++){
            // If the board is already in the cache and the new score is found for wider alpha beta window, update the entry
            if(table[idx + i].board == boardID && table[idx + i].depth <= depth && table[idx + i].upperBound <= upperBound && table[idx + i].lowerBound >= lowerBound){
                table[idx + i] = {boardID, best, score, depth, upperBound, lowerBound};
                return;
            }
        }

        for(int i = 0; i < bucket_size; i++){
            // If the bucket is empty or the depth is lower than the current entry, fill the bucket
            if(table[idx + i].depth < depth){
                if(table[idx + i].depth == UNSET_DEPTH) // Count the number of filled entries
                    filled++;
                table[idx + i] = {boardID, best, score, depth, upperBound, lowerBound};
                return;
            }
        }
	}
    inline const CacheEntry &get(const GameState &gameState) const {
        Board perspectiveBoard = gameState.getPerspectiveBoard();
        uint64_t hash = getHash(perspectiveBoard);
		size_t idx = 3 * (hash & (table_size-1));
        for(int i = 0; i < bucket_size; i++){
            if(table[idx + i].board == perspectiveBoard.getID()){
                return table[idx + i];
            }
        }
        return null_entry;
	}
	inline float fillRate() const { return static_cast<float>(filled) / static_cast<float>(table_size * 3); }
private:
	std::array<CacheEntry, bucket_size * table_size> table{};
	CacheEntry null_entry;
#ifdef MEASURE_CACHE_FILL
	size_t filled = 0;
#endif
};

#endif
