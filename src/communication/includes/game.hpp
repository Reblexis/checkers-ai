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

/**
 * @class Pos
 * @brief Represents a position on the board
 * @var x The row coordinate of the position (0-7)
 * @var y The column coordinate of the position (0-7)
 */
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

/**
 * @class Move
 * @brief Represents a move on the board
 * @var id The id of the move
 * @var path The steps of the move
 * @var rotated Whether the move is flipped (for black player)
 */
struct Move {
    piece_move id=0;
    std::vector<Pos> path;
    bool rotated = false;
    piece_move getSubMove(unsigned int index);
    piece_move getPieceMove() const;
};

using position = uint8_t;
using board_id = __int128;

/**
 * @class Board
 * @brief Represents the state of the board itself
 * @var whiteBitboard The bitboard of the white pieces
 * @var blackBitboard The bitboard of the black pieces
 */
class Board {
public:
    Board(bitboard_all whiteBitboard, bitboard_all blackBitboard);

    board_id getID() const;
    std::optional<Piece> getAt(Pos pos) const;
    Board getBoardRev() const;
    int allPiecesCount() const; ///< Returns the total number of pieces on the board
    int whitePiecesCount() const; ///< Returns the total number of white pieces on the board
    int blackPiecesCount() const; ///< Returns the total number of black pieces on the board
    int whiteKingsCount() const; ///< Returns the total number of white kings on the board
    int blackKingsCount() const; ///< Returns the total number of black kings on the board
    int whitePawnsCount() const; ///< Returns the total number of white pawns on the board
    int blackPawnsCount() const; ///< Returns the total number of black pawns on the board
    bitboard getWhitePieces() const; ///< Returns the bitboard for the white pieces
    bitboard getBlackPieces() const; ///< Returns the bitboard for the black pieces
    bitboard getWhiteKings() const; ///< Returns the bitboard for the white kings
    bitboard getBlackKings() const; ///< Returns the bitboard for the black kings
    const bitboard_all whiteBitboard; ///< The bitboard of the white pieces (first 32 bits) and kings (last 32 bits)
    const bitboard_all blackBitboard; ///< The bitboard of the black pieces (first 32 bits) and kings (last 32 bits)

    friend std::ostream& operator<<(std::ostream& os, const Board& obj);
    bool operator==(const Board& other) const;

private:
    static bitboard reverseBitboard(bitboard bitboardToReverse) ;
    static bitboard_all reverseBoard(bitboard_all boardToReverse) ;
};

Board getBoardFromStream(std::istream& inputStream);

/**
 * @class GameState
 * @brief Represents the state of the game
 * @var board The board of the game
 * @var nextBlack Whether the black plays next
 */
struct GameState {
public:
    const Board board;
    const bool nextBlack;

    GameState(Board board, bool nextBlack);
    Board getPerspectiveBoard() const; ///< Returns the board from the perspective of the next player
    std::span<const piece_move> getAvailableMoves() const; ///< Returns the available moves for the current player
    Move getMove(piece_move pieceMove) const; ///< Converts piece_move to Move class
    std::vector<Move> getAvailableMoves2() const; ///< Returns the available moves for the current player as a vector of Move

    friend std::ostream& operator<<(std::ostream& os, const GameState& obj); ///< Outputs the game state to the stream

private:
    void calculateAvailableMoves(); ///< Finds the available moves for the current player
    void searchMoves(piece_move currentMove, unsigned int jumpCount, position lastPos, bitboard curEnemyPieces, bool isKing, bitboard controlPieces); ///< Recursive function to find the available moves
    std::vector<piece_move> availableMoves; ///< The available moves for the current player
};

/**
 * @class Game
 * @brief Controls the game mechanics
 * @var gameHistory The states of the game
 */
class Game {
public:
    Game();
    explicit Game(const GameState& state); ///< Initializes the game with the given state
    void addGameState(const GameState& state); ///< Adds a new state to the game
    void undoMove(); ///< Undoes the last move
    void reset(const GameState& state); ///< Resets the game to the given state
    const GameState& getGameState() const; ///< Returns the current state of the game
    void makeMove(piece_move pieceMove, bool final=true); ///< Makes a move
    bool isFinished() const; ///< Returns whether the game has finished (no available moves)

private:
    std::vector<GameState> gameHistory;
};

#endif // GAME_HPP
