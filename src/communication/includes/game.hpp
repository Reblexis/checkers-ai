#ifndef GAME_HPP
#define GAME_HPP

#include <optional>
#include <vector>
#include <span>
#include <cstdint>
#include <functional>
#include <iostream>

#define CHECK_VALID_MOVES true

enum Piece { whitePawn, whiteKing, blackPawn, blackKing };

enum Direction { topLeft=1, topRight=2, bottomLeft=3, bottomRight=4 };

struct Pos {
    // Position in terms of the whole board including empty tiles
    unsigned int x;
    unsigned int y;
    Pos(unsigned int x, unsigned int y);
    Pos (unsigned int index);
    unsigned int indexFromPos() const;
    Direction getDirection(Pos pos) const;

    bool operator==(const Pos& other) const;
    Pos operator+(const Pos& other) const;
    Pos operator-(const Pos& other) const;
};

constexpr unsigned int BOARD_SIZE = 32;
using bitboard_all = uint64_t; // First 32 bits represent all pieces, last 32 bits represent kings
using bitboard = uint32_t;
/*
 * 0 1 2 3
 * 4 5 6 7
 * ...
 */
using piece_move = uint64_t;
struct Move {
    // Nice version of piece_move
    piece_move id;
    std::vector<Pos> path;
    piece_move getSubMove(unsigned int index);
};

using position = uint8_t;

class Board {
public:
    Board(const bitboard_all whiteBitboard, const bitboard_all blackBitboard);

    const std::optional<Piece> getAt(Pos pos) const;
    const Board getBoardRev() const;
    const int whitePiecesCount() const;
    const int blackPiecesCount() const;
    const int whiteKingsCount() const;
    const int blackKingsCount() const;
    const int whitePawnsCount() const;
    const int blackPawnsCount() const;
    const bitboard getWhitePieces() const;
    const bitboard getBlackPieces() const;
    const bitboard getWhiteKings() const;
    const bitboard getBlackKings() const;
    const bitboard_all whiteBitboard;
    const bitboard_all blackBitboard;

private:
    const bitboard reverseBitboard(const bitboard bitboardToReverse) const;
    const bitboard_all reverseBoard(const bitboard_all boardToReverse) const;
};

struct GameState {
public:
    const Board board;
    const bool nextBlack;
    uint64_t hash;

    GameState(Board board, bool nextBlack);
    std::span<const piece_move> getAvailableMoves() const;
    std::vector<Move> getAvailableMoves2() const;

private:
    void calculateAvailableMoves();
    std::vector<piece_move> availableMoves;
};

class Game {
public:
    Game(const GameState state);
    void addGameState(const GameState state);
    void undoMove();
    void reset(GameState state);
    const GameState& getGameState() const;
    void makeMove(piece_move pieceMove);

private:
    std::vector<GameState> gameHistory;
};

#endif // GAME_HPP
