#include <chrono>

#include "../communication/includes/debugging.hpp"

#include "../communication/includes/interface.hpp"
#include "../ai/includes/hyperparams.hpp"


uint64_t get_tree_size(Board &b, int depth){
    if (depth < 1)
        return 1;
    moveList ml = b.moves();
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
    Board b(0xfff00000, 0xfff);
    message("Running interface perft test", true);
    for (int d = 1; d < 13; d++) {
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t res = get_tree_size(b, d);
        auto end = std::chrono::high_resolution_clock::now();
        message("depth " + std::to_string(d) + ": " + std::to_string(res) + " nodes [took " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()) + "ms]", false);
    }
}

std::array<int, 2> test_game(const Board &original_board, int asettings[NUM_HYPERPARAMS], int bsettings[NUM_HYPERPARAMS], bool decide_by_num_pieces = false) {
    std::array<int, 2> res = {-1, -1};
    // res[id] = r corresponds to the game with id=id and result r means 0=draw, 1=win for player a, 2=loss for player a
    for (int game_id = 0; game_id < 2; game_id++) {
        // If game_id == 0, then A is black, otherwise A is white
        Board b(original_board);
        moveList ml = b.moves();
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
            // show Board
            ml = b.moves();
        }
        if(res[game_id] == -1){
            res[game_id] = (b.nextblack == game_id) ? 1 : 2;
        }
    }

    return res;
}

std::array<int, 5> play_test(int num_games, int asettings[NUM_HYPERPARAMS], int bsettings[NUM_HYPERPARAMS]) {
    // return {win_A_black, win_B_black, win_A_white, win_B_white, draw}
    message("Play test", true);
    Board b(0xfff00000, 0xfff);
    int win_A_black = 0;
    int win_B_black = 0;
    int win_A_white = 0;
    int win_B_white = 0;
    int draw = 0;
    int p2counter = 0;

    for(int i = 0; i < num_games; i++){
        // Do first 4 moves randomly
        for(int j = 0; j<4; j++){
            moveList ml = b.moves();
            move random_move = ml.begin()[rand() % ml.size()];
            b.play(random_move);
        }

        std :: array<int, 2> game_res = test_game(b, asettings, bsettings);

        draw += (game_res[0] == 0) + (game_res[1] == 0);
        win_A_black += (game_res[0] == 1);
        win_B_black += (game_res[0] == 2);
        win_A_white += (game_res[1] == 1);
        win_B_white += (game_res[1] == 2);

        message(".", false, false);

        b = Board(0xfff00000, 0xfff);
    }

    return {win_A_black, win_B_black, win_A_white, win_B_white, draw};
}

void test_brain_performance(int input_test_hyperparams[NUM_HYPERPARAMS] = nullptr, int num_games = 20)
{
    int test_hyperparams[NUM_HYPERPARAMS];
    for(int i = 0; i < NUM_HYPERPARAMS; i++)
        test_hyperparams[i] = input_test_hyperparams ? input_test_hyperparams[i] : allhyperparams[i];

    int total_wins = 0;
    int total_draws = 0;
    int total_losses = 0;

    for(int i = 0; i < TESTING_SAMPLES_NUM; i++)
    {
        Board b(0xfff00000, 0xfff);
        std::array<int, 5> result = play_test(num_games, test_hyperparams, testing_samples[i]);
        message("Result of test game " + std::to_string(i) + ":", true);
        message("Total games: " + std::to_string(num_games), false);
        message("Wins: " + std::to_string(result[0] + result[2]), false);
        message("Losses: " + std::to_string(result[1] + result[3]), false);
        message("Draws: " + std::to_string(result[4]), false);

        total_draws += result[4];
        total_losses += result[1] + result[3];
        total_wins += result[0] + result[2];
    }

    message("Final result:", true);
    message("Total games: " + std::to_string(num_games*TESTING_SAMPLES_NUM), false);
    message("Wins: " + std::to_string(total_wins), false);
    message("Losses: " + std::to_string(total_losses), false);
    message("Draws: " + std::to_string(total_draws), false);
}
