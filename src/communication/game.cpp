#include <optional>
#include <vector>
#include <span>
#include <cstdint>
#include <execution>

#define CHECK_VALID_MOVES true

enum Piece{whitePawn, whiteKing, blackPawn, blackKing};

constexpr unsigned int BOARD_SIZE = 32;
using bitboard_all = uint64_t; // First 32 bits represent all pieces, last 32 bits represent kings
/*
     * The bits represent the board in the following way:
     * 0 1 2 3
     * 4 5 6 7 ...
     * .
     * .
     * .
     * For kings, all numbers are offset by 32
 */
using bitboard = uint32_t;
using piece_move = uint64_t;
using position = uint8_t;
/*
 * First 5 bits represent the starting square. Afterward follows sequence of 3-bit numbers representing the direction of
 * each move. 0 - end of move sequence, 1 - top left, 2 - top right, 3 - bottom left, 4 - bottom right. This is from the perspective of the playing agent.
 */
class Board{
    /*
     * By default, white is on the bottom of the board.
     */
private:
    const bitboard reverseBitboard(const bitboard bitboardToReverse){
        bitboard reversedBitboard = reversedBitboard;
        reversedBitboard = (reversedBitboard & 0x55555555) << 1 | (reversedBitboard & 0xAAAAAAAA) >> 1;
        reversedBitboard = (reversedBitboard & 0x33333333) << 2 | (reversedBitboard & 0xCCCCCCCC) >> 2;
        reversedBitboard = (reversedBitboard & 0x0F0F0F0F) << 4 | (reversedBitboard & 0xF0F0F0F0) >> 4;
        reversedBitboard = (reversedBitboard & 0x00FF00FF) << 8 | (reversedBitboard & 0xFF00FF00) >> 8;
        reversedBitboard = (reversedBitboard & 0x0000FFFF) << 16 | (reversedBitboard & 0xFFFF0000) >> 16;
        return reversedBitboard;
    }

    const bitboard_all reverseBoard(bitboard_all boardToReverse){
        bitboard pieces = reverseBitboard(boardToReverse&0xffffffff);
        bitboard kings = reverseBitboard(boardToReverse>>32);
        return (((bitboard_all)kings)<<32) | pieces;
    }

public:
    // TODO rename white to control and black to enemy
    const bitboard_all controlBitboard = 0xfff00000; // The starting pawn setup
    const bitboard_all enemyBitboard = 0xfff;
    const bool blackPerspective = true;

    Board(const bitboard_all controlBitboard, const bitboard_all enemyBitboard, const bool blackPerspective)
            : controlBitboard(controlBitboard), enemyBitboard(enemyBitboard), blackPerspective(blackPerspective) {
    }

    std::optional<Piece> getAt(int x, int y){
        // Y corresponds to the row, X corresponds to the column
        uint8_t pos = x + y*4;
        if(controlBitboard&(1<<(BOARD_SIZE+pos))){
            return Piece::whiteKing;
        }
        if(enemyBitboard&(1<<(BOARD_SIZE+pos))){
            return Piece::blackKing;
        }
        if(controlBitboard&(1<<pos)){
            return Piece::whitePawn;
        }
        if(enemyBitboard&(1<<pos)){
            return Piece::blackPawn;
        }
        return std::nullopt;
    }

    void reset(){
        controlBitboard = 0x00000000fff00000; // The starting pawn setup
        enemyBitboard = 0x0000000000000fff;
    }

    Board getBoardRev(){
        return Board(reverseBoard(enemyBitboard), reverseBoard(controlBitboard), !blackPerspective);
    }

    const int whitePiecesCount() const{
        return __builtin_popcount(controlBitboard&0xffffffff);
    }
    const int blackPiecesCount() const{
        return __builtin_popcount(enemyBitboard&0xffffffff);
    }
    const int whiteKingsCount() const{
        return __builtin_popcount(controlBitboard>>32);
    }
    const int blackKingsCount() const{
        return __builtin_popcount(enemyBitboard>>32);
    }
    const int whitePawnsCount() const{
        return whitePiecesCount() - whiteKingsCount();
    }
    const int blackPawnsCount() const{
        return blackPiecesCount() - blackKingsCount();
    }

    const bitboard getWhitePieces() const{
        return controlBitboard&0xffffffff;
    }
    const bitboard getBlackPieces() const{
        return enemyBitboard&0xffffffff;
    }
    const bitboard getWhiteKings() const{
        return controlBitboard>>32;
    }
    const bitboard getBlackKings() const{
        return enemyBitboard>>32;
    }
};

struct GameState{
public:
    GameState(Board board, bool nextBlack)
            : board(board), nextBlack(nextBlack){
        calculateAvailableMoves();
    }
    const Board board;
    const bool nextBlack = true;
    uint64_t hash = 0; // TODO: Move this to cache.cpp

    std::span<piece_move> getAvailableMoves() const{
        return availableMoves;
    }

    const Board& getBoard() const{
        return board;
    }
private:
    std::vector<piece_move> availableMoves;

    void calculateAvailableMoves()
    {
        availableMoves.clear();

        std::array<bitboard_all, 2> boards = nextBlack ? board.getBoardsRev() : board.getBoards();
        bitboard_all controlBitboard = boards[0]; // The bitboard_all of the pieces that can move next
        bitboard_all enemyBitboard = boards[1]; // The bitboard_all of the pieces that can be captured

        bitboard controlPieces = controlBitboard&0xffffffff;
        bitboard controlKings = controlBitboard>>32;
        bitboard enemyPieces = enemyBitboard&0xffffffff;

        std::function<void(piece_move, unsigned int, position, bitboard, bool)> calculateMoves = [&](piece_move currentMove, unsigned int jumpCount, position lastPos, bitboard curEnemyPieces, bool isKing){
            bool anyJumps = false;
            isKing |= (lastPos<8);
            bitboard anyPieces = controlPieces | curEnemyPieces;
            // Jump top-left
            if(lastPos > 15 && lastPos % 8 > 1 && (curEnemyPieces&(1<<(lastPos-9)) && !(anyPieces&(1<<(lastPos-18))))){
                calculateMoves(currentMove | (1<<(jumpCount*3 + 5)), jumpCount+1, lastPos-18, curEnemyPieces^(1<<(lastPos-9)), isKing);
                anyJumps = true;
            }
            // Jump top-right
            if(lastPos > 15 && lastPos % 8 < 6 && (curEnemyPieces&(1<<(lastPos-7)) && !(anyPieces&(1<<(lastPos-14))))){
                calculateMoves(currentMove | (2<<(jumpCount*3 + 5)), jumpCount+1, lastPos-14, curEnemyPieces^(1<<(lastPos-7)), isKing);
                anyJumps = true;
            }
            // Jump bottom-left
            if(isKing && lastPos < 48 && lastPos % 8 > 1 && (curEnemyPieces&(1<<(lastPos+7)) && !(anyPieces&(1<<(lastPos+14))))){
                calculateMoves(currentMove | (3<<(jumpCount*3 + 5)), jumpCount+1, lastPos+14, curEnemyPieces^(1<<(lastPos+7)), isKing);
                anyJumps = true;
            }
            // Jump bottom-right
            if(isKing && lastPos < 48 && lastPos % 8 < 6 && (curEnemyPieces&(1<<(lastPos+9)) && !(anyPieces&(1<<(lastPos+18))))){
                calculateMoves(currentMove | (4<<(jumpCount*3 + 5)), jumpCount+1, lastPos+18, curEnemyPieces^(1<<(lastPos+9)), isKing);
                anyJumps = true;
            }

            if(!anyJumps){
                availableMoves.push_back(currentMove);
            }
        };

        for(int i = 0; i < BOARD_SIZE; i++){
            if(controlKings&(1<<i))
            {
                calculateMoves(i, 0, i, enemyPieces, true);
            }
        }
        if(availableMoves.size() > 0)
            return;

        bitboard controlPawns = controlPieces^controlKings;
        for(int i = 0; i < BOARD_SIZE; i++){
            if((controlPawns)&(1<<i))
            {
                calculateMoves(i, 0, i, enemyPieces, false);
            }
        }
        if(availableMoves.size() > 0)
            return;

        bitboard anyPieces = controlPieces | enemyPieces;
        for(int i = 0; i < BOARD_SIZE; i++){
            if(!controlPieces&(1<<i))
                continue;

            // Move top-left
            if(i>7 && i%8>0 && !(anyPieces&(1<<(i-9))))
            {
                availableMoves.push_back(i | (1<<5));
            }
            // Move top-right
            if(i>7 && i%8<7 && !(anyPieces&(1<<(i-7))))
            {
                availableMoves.push_back(i | (2<<5));
            }

            if(controlKings&(1<<i))
            {
                // Move bottom-left
                if(i<56 && i%8>0 && !(anyPieces&(1<<(i+7))))
                {
                    availableMoves.push_back(i | (3<<5));
                }
                // Move bottom-right
                if(i<56 && i%8<7 && !(anyPieces&(1<<(i+9))))
                {
                    availableMoves.push_back(i | (4<<5));
                }
            }
        }
    }
};

class Game{
private:
    GameState gameState;
    std::vector<GameState> gameHistory;

public:
    Game(){
        reset(GameState{});
    }

    void addGameState()
    {
        gameHistory.push_back(gameState);
    }

    void undoMove()
    {
        if(gameHistory.size() == 1)
            throw std::runtime_error("Cannot undo move. No moves have been made.");

        gameHistory.pop_back();
        gameState = gameHistory.back();
    }

    void reset(GameState state)
    {
        gameState = state;
        gameHistory.clear();
        addGameState();
    }

    const GameState& getGameState() const
    {
        return gameState;
    }

    std::span<piece_move> getAvailableMoves(){
        return gameState.getAvailableMoves();
    }

    void makeMove(piece_move pieceMove){
        unsigned int currentPos = pieceMove&0x1f;

        pieceMove >>= 5;
        Board perspectiveBoard = gameState.nextBlack ? gameState.board.getBoardRev() : gameState.board;
        bitboard_all controlBitboard = perspectiveBoard.controlBitboard;
        bitboard_all enemyBitboard = perspectiveBoard.enemyBitboard;
        bool isKing = controlBitboard&(1<<(currentPos+32));
        controlBitboard &= (~(1<<currentPos))&(~(1<<(currentPos+32)));

        while(pieceMove){
            unsigned int direction = pieceMove&0x7;
            unsigned int startPos = currentPos;
            if(direction==1)
                currentPos-=9;
            else if(direction==2)
                currentPos-=7;
            else if(direction==3)
                currentPos+=7;
            else
                currentPos+=9;

#if CHECK_VALID_MOVES
            if(currentPos>63 || currentPos<0 || controlBitboard&(1<<currentPos) || (direction>=3 && !isKing))
                throw std::runtime_error("Invalid move.");
#endif

            // Do a jump if standing on an enemy piece
            if(enemyBitboard&(1<<currentPos)) {
                enemyBitboard &= (~(1<<currentPos))&(~(1<<(currentPos+32)));
                currentPos += (currentPos - startPos);
            }

#if CHECK_VALID_MOVES
            if(currentPos>63 || currentPos<0 || (controlBitboard|enemyBitboard)&(1<<currentPos))
                throw std::runtime_error("Invalid move. Incorrect jump.");
#endif

            if(currentPos < 8)
                isKing = true;

            pieceMove >>= 3;
        }

        controlBitboard |= (1<<currentPos);
        if(isKing)
            controlBitboard |= (1<<(currentPos+32));

        Board
        const GameState newGameState(newBoard, !gameState.nextBlack);
        gameState = newGameState;

        addGameState();
    }
};