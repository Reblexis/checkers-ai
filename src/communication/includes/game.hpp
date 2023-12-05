#ifndef CHECKERS_GAME_H
#define CHECKERS_GAME_H

#include <optional>
#include <vector>
#include <span>
#include <cstdint>
#include <functional>
#include <array>
#include <stdexcept>

#define CHECK_VALID_MOVES true

enum Piece { whitePawn, whiteKing, blackPawn, blackKing };

constexpr unsigned int BOARD_SIZE = 32;
using board_state = uint64_t; // First 32 bits represent all pieces, last 32 bits represent kings
using bitboard = uint32_t;
using piece_move = uint64_t;
using position = uint8_t;

class Board {
private:
    board_state whiteBoard = 0xfff00000; // The starting pawn setup
    board_state blackBoard = 0xfff;

    bitboard reverseBitboard(bitboard bitboardToReverse);
    board_state reverseBoard(board_state boardToReverse);

public:
    Board();
    std::optional<Piece> getAt(int x, int y);
    void reset();
    Board(const Board& other);
    void setBoard(board_state white, board_state black);
    void setBoardRev(board_state black, board_state white);
    std::array<board_state, 2> getBoards();
    std::array<board_state, 2> getBoardsRev();
};

struct GameState {
    Board board{};
    bool nextBlack = true;
};

class Game {
private:
    Board board;
    bool nextBlack = true;
    std::vector<GameState> gameHistory;
    std::vector<piece_move> availableMoves;

    void calculateAvailableMoves();

public:
    Game();
    void addGameState();
    void undoMove();
    void reset(GameState state);
    std::span<piece_move> getAvailableMoves();
    void makeMove(piece_move pieceMove);
};

#endif // CHECKERS_GAME_H