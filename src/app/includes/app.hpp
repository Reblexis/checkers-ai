#ifndef APP_HPP
#define APP_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

#include "../../communication/includes/game.hpp"
#include "../../ai/includes/agent.hpp"
#include "../../communication/includes/debugging.hpp"

const unsigned int TILE_SIZE = 80;
const unsigned int BOARD_DIMENSION = 8 * TILE_SIZE;

const sf::Color WHITE_TILE_COLOR = sf::Color(245, 245, 220); ///< Color of the white tiles
const sf::Color BLACK_TILE_COLOR = sf::Color(160, 82, 45); ///< Color of the black tiles
const sf::Color SELECTED_TILE_COLOR = sf::Color(240, 240, 10, 90); ///< Color of the selected tile visualization
const sf::Color POSSIBLE_MOVE_COLOR = sf::Color(10, 240, 10, 90); ///< Color of the possible move visualization
const sf::Color LAST_MOVE_COLOR = sf::Color(224, 85, 169, 90); ///< Color of the last move visualization
const sf::Color BLACK_PIECE_COLOR = sf::Color(0, 0,0); ///< Color of the black pieces
const sf::Color WHITE_PIECE_COLOR = sf::Color(200,200,200); ///< Color of the white pieces

/**
 * @class UI
 * @brief Class for managing the user interface of the game.
 *
 * The UI class is used to manage certain aspects needed for the user interface of the game.
 *
 * @var selectedSquare The currently selected square
 * @var possibleMoves The possible moves for the selected piece
 * @var lastMove The last move made
 */
class UI{
public:
    std::optional<Pos> selectedSquare;
    std::vector<Pos> possibleMoves;
    std::vector<Pos> lastMove;
};

/**
 * @class PieceSprite
 * @brief Sprite class for the pieces.
 *
 * @var isWhitePiece Whether the piece is white or black
 * @var isKing Whether the piece is a king
 */
class PieceSprite : public sf::CircleShape {
public:
    PieceSprite(Piece piece);

    void setPositionCentered(int x, int y); ///< Sets the position of the piece centered on the given coordinates

    bool isWhitePiece;
    bool isKing;
};

/**
 * @class App
 * @brief Class for managing the GUI.
 *
 * The App class is used to manage the GUI of the game for the player and also for visualization of the game as such.
 *
 * @var window The window of the application
 * @var ui The user interface of the application
 */
class App {
public:
    bool refresh(const Game &game);
    Move getMove(Game &game, Timer& timer);
    void launch(const std::string& title="Checkers"); ///< Launches the application

private:
    sf::RenderWindow window;
    UI ui;

    void drawWindow(const Game &game);
    void drawUI();
    void drawBoard(const Board &board);
    void drawPieces(const Board &board);
    void highlightField(Pos pos, sf::Color color);
};

#endif // APP_HPP
