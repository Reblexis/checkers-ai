/*
#include "../communication/includes/debugging.hpp"

#include "../communication/includes/simulateGame.hpp"
#include "../ai/includes/hyperparameters.hpp"
#include "../ai/includes/search_algorithms.hpp"
#include "../ai/includes/evaluation.hpp"
#include "../ai/includes/agent.hpp"

void interfaceTest() {
    message("Running interface test", true);

    Board board(3430946816, 524288, true, 2048);
    srand(532904124);
    message(board.visualize());
    while (1){
        moveList ml = board.moves();
        for (move m : ml) {
            message(visualizeMove(m) );
        }
        if (!ml.size())
            break;
        move *selected = ml.begin() + (rand() % ml.size());
        board.play(*selected);
        message("selected: " + visualizeMove(*selected), false);
    }
}


void searchAlgorithmTest(Agent &agent1, Agent &agent2){
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

        std::pair<int, move> chosenMove = (b.nextblack ? agent1 : agent2).findBestMove(b);

        message("score: " + std::to_string(chosenMove.first) + "\nselected: " + visualizeMove(chosenMove.second));
        b.play(chosenMove.second);
        message(b.visualize());
    }
    message("Game over", true);
    message(b.visualize());
}

*/