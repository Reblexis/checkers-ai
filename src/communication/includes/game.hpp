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

constexpr unsigned int BOARD_SIZE = 32;
using bitboard_all = uint64_t; // First 32 bits represent all pieces, last 32 bits represent kings
using bitboard = uint32_t;
using piece_move = uint64_t;
using position = uint8_t;

class Board {
public:
    Board(const bitboard_all whiteBitboard, const bitboard_all blackBitboard);

    const std::optional<Piece> getAt(int x, int y) const;
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
