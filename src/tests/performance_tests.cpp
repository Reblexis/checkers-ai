#include <chrono>

#include "../communication/includes/debugging.hpp"

#include "../communication/includes/game.hpp"
#include "../ai/includes/hyperparameters.hpp"
#include "../ai/includes/agent.hpp"
#include "../ai/includes/search_algorithms.hpp"
#include "../ai/includes/evaluation.hpp"


uint64_t getTreeSize(Game &game, int depth){
    if (depth < 1)
        return 1;
    std::span<const piece_move> possibleMoves = game.getGameState().getAvailableMoves();
    uint64_t counter = 1;
    for (const piece_move move : possibleMoves) {
        game.makeMove(move);
        counter += getTreeSize(game, depth - 1);
        game.undoMove();
    }
    return counter;
}

void interfacePerformanceTest() {
    Game game(GameState(Board(0xfff00000, 0xfff), true));
    message("Running interface perft test", true);
    for (int d = 1; d < 13; d++) {
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t res = getTreeSize(game, d);
        auto end = std::chrono::high_resolution_clock::now();
        message("depth " + std::to_string(d) + ": " + std::to_string(res) + " nodes [took " +
                std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms]",
                false);
    }
}