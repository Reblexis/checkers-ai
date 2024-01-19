#ifndef GAME_HPP
#define GAME_HPP

#include <optional>
#include <vector>
#include <span>
#include <cstdint>
#include <functional>
#include <iostream>

#define CHECK_VALID_MOVES true

constexpr int NUM_SQUARES = 32;

enum Piece { whitePawn=0, whiteKing=1, blackPawn=2, blackKing=3 };

enum Direction { topLeft=1, topRight=2, bottomLeft=3, bottomRight=4 };

struct Pos {
    // Position in terms of the whole board including empty tiles
    int x;
    int y;
    Pos(int x, int y);
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
    piece_move id;
    std::vector<Pos> path;
    bool rotated = false;
    piece_move getSubMove(unsigned int index);
    piece_move getPieceMove() const;
};

using position = uint8_t;

class Board {
public:
    Board(bitboard_all whiteBitboard, bitboard_all blackBitboard);

    std::optional<Piece> getAt(Pos pos) const;
    Board getBoardRev() const;
    int allPiecesCount() const;
    int whitePiecesCount() const;
    int blackPiecesCount() const;
    int whiteKingsCount() const;
    int blackKingsCount() const;
    int whitePawnsCount() const;
    int blackPawnsCount() const;
    bitboard getWhitePieces() const;
    bitboard getBlackPieces() const;
    bitboard getWhiteKings() const;
    bitboard getBlackKings() const;
    const bitboard_all whiteBitboard;
    const bitboard_all blackBitboard;

    friend std::ostream& operator<<(std::ostream& os, const Board& obj);

private:
    static bitboard reverseBitboard(bitboard bitboardToReverse) ;
    static bitboard_all reverseBoard(bitboard_all boardToReverse) ;
};

Board getBoardFromStream(std::istream& inputStream);

struct GameState {
public:
    const Board board;
    const bool nextBlack;

    GameState(Board board, bool nextBlack);
    Board getPerspectiveBoard() const;
    std::span<const piece_move> getAvailableMoves() const;
    Move getMove(piece_move pieceMove) const;
    std::vector<Move> getAvailableMoves2() const;

    friend std::ostream& operator<<(std::ostream& os, const GameState& obj);

private:
    void calculateAvailableMoves();
    std::vector<piece_move> availableMoves;
};

class Game {
public:
    Game();
    explicit Game(const GameState& state);
    void addGameState(const GameState& state);
    void undoMove();
    void reset(const GameState& state);
    const GameState& getGameState() const;
    void makeMove(piece_move pieceMove, bool final=true);
    bool isFinished() const;

private:
    std::vector<GameState> gameHistory;
};

#endif // GAME_HPP
