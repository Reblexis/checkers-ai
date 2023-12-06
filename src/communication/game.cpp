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
    const bitboard reverseBitboard(const bitboard bitboardToReverse) const{
        bitboard reversedBitboard = reversedBitboard;
        reversedBitboard = (reversedBitboard & 0x55555555) << 1 | (reversedBitboard & 0xAAAAAAAA) >> 1;
        reversedBitboard = (reversedBitboard & 0x33333333) << 2 | (reversedBitboard & 0xCCCCCCCC) >> 2;
        reversedBitboard = (reversedBitboard & 0x0F0F0F0F) << 4 | (reversedBitboard & 0xF0F0F0F0) >> 4;
        reversedBitboard = (reversedBitboard & 0x00FF00FF) << 8 | (reversedBitboard & 0xFF00FF00) >> 8;
        reversedBitboard = (reversedBitboard & 0x0000FFFF) << 16 | (reversedBitboard & 0xFFFF0000) >> 16;
        return reversedBitboard;
    }

    const bitboard_all reverseBoard(const bitboard_all boardToReverse) const{
        bitboard pieces = reverseBitboard(boardToReverse&0xffffffff);
        bitboard kings = reverseBitboard(boardToReverse>>32);
        return (((bitboard_all)kings)<<32) | pieces;
    }

public:
    const bitboard_all whiteBitboard = 0xfff00000; // The starting pawn setup
    const bitboard_all blackBitboard = 0xfff;

    Board(const bitboard_all whiteBitboard, const bitboard_all blackBitboard)
            : whiteBitboard(whiteBitboard), blackBitboard(blackBitboard) {
    }

    const std::optional<Piece> getAt (int x, int y) const{
        // Y corresponds to the row, X corresponds to the column
        uint8_t pos = x + y*4;
        if(whiteBitboard&(1<<(BOARD_SIZE+pos))){
            return Piece::whiteKing;
        }
        if(blackBitboard&(1<<(BOARD_SIZE+pos))){
            return Piece::blackKing;
        }
        if(whiteBitboard&(1<<pos)){
            return Piece::whitePawn;
        }
        if(blackBitboard&(1<<pos)){
            return Piece::blackPawn;
        }
        return std::nullopt;
    }

    const Board getBoardRev() const{
        return Board(reverseBoard(blackBitboard), reverseBoard(whiteBitboard));
    }

    const int whitePiecesCount() const{
        return __builtin_popcount(whiteBitboard&0xffffffff);
    }
    const int blackPiecesCount() const{
        return __builtin_popcount(blackBitboard&0xffffffff);
    }
    const int whiteKingsCount() const{
        return __builtin_popcount(whiteBitboard>>32);
    }
    const int blackKingsCount() const{
        return __builtin_popcount(blackBitboard>>32);
    }
    const int whitePawnsCount() const{
        return whitePiecesCount() - whiteKingsCount();
    }
    const int blackPawnsCount() const{
        return blackPiecesCount() - blackKingsCount();
    }

    const bitboard getWhitePieces() const{
        return whiteBitboard&0xffffffff;
    }
    const bitboard getBlackPieces() const{
        return blackBitboard&0xffffffff;
    }
    const bitboard getWhiteKings() const{
        return whiteBitboard>>32;
    }
    const bitboard getBlackKings() const{
        return blackBitboard>>32;
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

    std::span<const piece_move> getAvailableMoves() const{
        return std::span<const piece_move>(availableMoves);
    }

    const Board& getBoard() const{
        return board;
    }

private:
    std::vector<piece_move> availableMoves;

    void calculateAvailableMoves()
    {
        availableMoves.clear();

        Board boards = nextBlack ? board.getBoardRev() : board;
        bitboard_all controlBitboard = boards.whiteBitboard; // The bitboard_all of the pieces that can move next
        bitboard_all enemyBitboard = boards.blackBitboard; // The bitboard_all of the pieces that can be captured

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
    std::vector<GameState> gameHistory;

public:
    Game(const GameState state){
        addGameState(state);
    }

    void addGameState(const GameState state)
    {
        gameHistory.emplace_back(gameHistory.back());
    }

    void undoMove()
    {
        if(gameHistory.size() == 1)
            throw std::runtime_error("Cannot undo move. No moves have been made.");

        gameHistory.pop_back();
    }

    void reset(GameState state)
    {
        gameHistory.clear();
        addGameState(state);
    }

    const GameState& getGameState() const
    {
        return gameHistory.back();
    }

    void makeMove(piece_move pieceMove){
        unsigned int currentPos = pieceMove&0x1f;

        pieceMove >>= 5;
        Board perspectiveBoard = gameHistory.back().nextBlack ? gameHistory.back().board.getBoardRev() : gameHistory.back().board;
        bitboard_all controlBitboard = perspectiveBoard.whiteBitboard;
        bitboard_all enemyBitboard = perspectiveBoard.blackBitboard;
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

        Board newBoard(controlBitboard, enemyBitboard);

        const GameState newGameState(gameHistory.back().nextBlack ? newBoard: newBoard.getBoardRev(), !gameHistory.back().nextBlack);
        addGameState(newGameState);
    }
};