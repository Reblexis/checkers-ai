#include "includes/console_interface.hpp"

ConsoleInterface::ConsoleInterface(Agent *agent) : agent(agent) {}

void ConsoleInterface::run(){
    long long timeLimit; // in milliseconds
    std::cin>>timeLimit;
    std::string color;
    std::cin>>color;
    bool isBlack = color == "black";

    agent->initialize(timeLimit, isBlack);

    while(true)
    {
        int timeLeft; // in milliseconds
        std::cin>>timeLeft;

        std::vector<std::string> boardState(8);
        for(unsigned int i = 0; i < 8; i++) {
            for (unsigned int j = 0; j < 8; j++){
                char c;
                std::cin>>c;
                boardState[i].push_back(c);
            }
        }

        bitboard_all whiteBitboard = 0;
        bitboard_all blackBitboard = 0;

        for(unsigned long long i = 0; i<32; i++)
        {
            Pos pos(i);
            whiteBitboard |= (long long)(tolower(boardState[pos.y][pos.x]) == 'w') << i;
            whiteBitboard |= (long long)(boardState[pos.y][pos.x] == 'W') << (i + 32);
            blackBitboard |= (long long)(tolower(boardState[pos.y][pos.x]) == 'b') << i;
            blackBitboard |= (long long)(boardState[pos.y][pos.x] == 'B') << (i + 32);
        }

        Board board(whiteBitboard, blackBitboard);
        GameState gameState(board, isBlack);
        Game game(gameState);

        Timer timer(timeLeft);
        timer.resume();
        std::pair<int, piece_move> bestMove = agent->findBestMove(game, timer);
        game.makeMove(bestMove.second);

        Move madeMove = gameState.getMove(bestMove.second);

        std::cout<<madeMove.path.size()<<" ";
        for(Pos pos: madeMove.path)
        {
            std::cout<<pos.x<<" "<<pos.y<<" ";
        }
        std::cout<<std::endl;

       // std::cout<<simulateGame.getGameState().board;
    }
}




