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
    explicit Pos (unsigned int index);
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
    Board(bitboard_all whiteBitboard, bitboard_all blackBitboard);

    std::optional<Piece> getAt(Pos pos) const;
    Board getBoardRev() const;
    unsigned int whitePiecesCount() const;
    unsigned int blackPiecesCount() const;
    unsigned int whiteKingsCount() const;
    unsigned int blackKingsCount() const;
    unsigned int whitePawnsCount() const;
    unsigned int blackPawnsCount() const;
    bitboard getWhitePieces() const;
    bitboard getBlackPieces() const;
    bitboard getWhiteKings() const;
    bitboard getBlackKings() const;
    const bitboard_all whiteBitboard;
    const bitboard_all blackBitboard;

private:
    static bitboard reverseBitboard(bitboard bitboardToReverse) ;
    static bitboard_all reverseBoard(bitboard_all boardToReverse) ;
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
    explicit Game(const GameState& state);
    void addGameState(const GameState& state);
    void undoMove();
    void reset(const GameState& state);
    const GameState& getGameState() const;
    void makeMove(piece_move pieceMove);

private:
    std::vector<GameState> gameHistory;
};

#endif // GAME_HPP
