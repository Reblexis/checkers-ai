#include "includes/console_interface.hpp"

ConsoleInterface::ConsoleInterface(Agent *agent) : agent(agent) {}

void ConsoleInterface::run(){
    while(true){
        std::string color;
        std::cin>>color;
        if(color == "exit")
            break;

        bool nextBlack = color == "black";

        std::vector<std::string> boardState(8);
        for(unsigned int i = 0; i < 8; i++)
        {
            std::cin>>boardState[i];
        }

        bitboard_all whiteBitboard = 0;
        bitboard_all blackBitboard = 0;

        for(unsigned int i = 0; i<32; i++)
        {
            Pos pos(i);
            whiteBitboard |= (tolower(boardState[pos.y][pos.x]) == 'w') << i;
            whiteBitboard |= (boardState[pos.y][pos.x] == 'W') << (i + 32);
            blackBitboard |= (tolower(boardState[pos.y][pos.x]) == 'b') << i;
            blackBitboard |= (boardState[pos.y][pos.x] == 'B') << (i + 32);
        }

        Board board(whiteBitboard, blackBitboard);
        GameState gameState(board, nextBlack);
        Game game(gameState);

        std::pair<int, piece_move> bestMove = agent->findBestMove(game);
        game.makeMove(bestMove.second);

        Move madeMove = gameState.getMove(bestMove.second);

        std::cout<<madeMove.path.size()<<std::endl;
        for(Pos pos: madeMove.path)
        {
            std::cout<<pos.x<<" "<<pos.y<<" ";
        }
        std::cout<<std::endl;

        Board boardAfterMove = game.getGameState().board;
        std::cout<<boardAfterMove<<std::endl;
    }
}




