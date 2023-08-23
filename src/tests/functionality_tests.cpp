#include "../communication/includes/debugging.hpp"

#include "../communication/includes/interface.hpp"
#include "../ai/includes/hyperparameters.hpp"
#include "../ai/includes/search_algorithms.hpp"

void interface_test() {
    message("Running interface test", true);

    Board b(3430946816, 524288, true, 2048);
    srand(532904124);
    std::cout << b.visualize();
    while (1){
        moveList ml = b.moves();
        for (move m : ml) {
            std::cout << visualizeMove(m);
        }
        if (!ml.size())
            break;
        move *selected = ml.begin() + (rand() % ml.size());
        b.play(*selected);
        message("selected: " + visualizeMove(*selected), false);
    }
}


void search_algorithm_test(const int settings_a[NUM_HYPERPARAMS], const int settings_b[NUM_HYPERPARAMS]) {
    // settings_a corresponds to the first player (black)

    Board b(0xfff00000, 0xfff);

    message("Running search algorithm test", true);
    message(b.visualize());

    int num_moves = 300;

    while(true){
        if (!num_moves--){
            message("Moves limit reached", true);
            break;
        }
        moveList ml = b.moves();
        if (!ml.size())
            break;

        for(int i = 0; i < NUM_HYPERPARAMS; i++)
            currenthyperparams[i] = b.nextblack ? settings_a[i] : settings_b[i];

        auto selected = findmove(b);
        message("score: " + std::to_string(selected.first) + "\nselected: " + visualizeMove(selected.secoond));
        b.play(selected.second);
        message(b.visualize());
    }
    message("Game over", true);
    message(b.visualize());
}

