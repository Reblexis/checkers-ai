#include <chrono>

#include "../communication/includes/debugging.hpp"

#include "../ai/includes/interface.hpp"

uint64_t get_tree_size(board &b, int depth){
    if (depth < 1)
        return 1;
    movelist ml = b.moves();
    if (depth < 2)
        return ml.size();
    uint64_t counter = 1;
    for (move m : ml) {
        b.play(m);
        counter += get_tree_size(b, depth-1);
        b.undo();
    }
    return counter;
}

void interface_performance_test() {
    board b(0xfff00000, 0xfff);
    message("Running interface perft test", true);
    for (int d = 1; d < 13; d++) {
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t res = get_tree_size(b, d);
        auto end = std::chrono::high_resolution_clock::now();
        message("depth " + std::to_string(d) + ": " + std::to_string(res) + " nodes [took " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()) + "ms]", false);
    }
}

std::array<int, 2> test_game(const board &original_board, int asettings[NUM_HYPERPARAMS], int bsettings[NUM_HYPERPARAMS], bool decide_by_num_pieces = false) {
    std::array<int, 2> res = {-1, -1};
    // res[id] = r corresponds to the game with id=id and result r means 0=draw, 1=win for player a, 2=loss for player a
    for (int game_id = 0; game_id < 2; game_id++) {
        // If game_id == 0, then A is black, otherwise A is white
        board b(original_board);
        movelist ml = b.moves();
        int limit = 0;
        while (ml.size()) {
            if (limit++ > 100) {
                // Decide by number of pieces
                res[game_id] = 0;

                if(decide_by_num_pieces) {
                    if (__builtin_popcountll(b.b) == __builtin_popcountll(b.w))
                        res[game_id] = 0;
                    else
                        res[game_id] = (__builtin_popcountll(b.b) > __builtin_popcountll(b.w)) ? game_id + 1 : 2 - game_id;
                }

                break;
            }

            if (b.nextblack != game_id) {
                // Current player - A
                for(int i = 0; i < NUM_HYPERPARAMS; i++)
                    currenthyperparams[i] = asettings[i];
            } else {
                // Current player - B
                for(int i = 0; i < NUM_HYPERPARAMS; i++)
                    currenthyperparams[i] = bsettings[i];
            }
            move m = findmove(b).second;
            if (!m)
                break;
            b.play(m);
            // show board
            ml = b.moves();
        }
        if(res[game_id] == -1){
            res[game_id] = (b.nextblack == game_id) ? 1 : 2;
        }
    }

    return res;
}
