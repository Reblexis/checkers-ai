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

const sf::Color WHITE_TILE_COLOR = sf::Color(245, 245, 220);
const sf::Color BLACK_TILE_COLOR = sf::Color(160, 82, 45);
const sf::Color SELECTED_TILE_COLOR = sf::Color(240, 240, 10, 90);
const sf::Color POSSIBLE_MOVE_COLOR = sf::Color(10, 240, 10, 90);
const sf::Color LAST_MOVE_COLOR = sf::Color(224, 85, 169, 90);
const sf::Color BLACK_PIECE_COLOR = sf::Color(0, 0,0);
const sf::Color WHITE_PIECE_COLOR = sf::Color(200,200,200);

class UI{
public:
    std::optional<Pos> selectedSquare;
    std::vector<Pos> possibleMoves;
    std::vector<Pos> lastMove;
};

class PieceSprite : public sf::CircleShape {
public:
    PieceSprite(Piece piece);

    void setPositionCentered(int x, int y);

    bool isWhitePiece;
    bool isKing;
};

class App {
public:
    void drawBoard(const Board &board);
    void drawPieces(const Board &board);
    void drawUI(const UI &ui);
    void drawWindow(const Game &game, const UI &ui);
    void gameLoop(Game& game, const Agent* agent1, const Agent* agent2, UI &ui);
    void launch();

private:
    sf::RenderWindow window;

    void highlightField(Pos pos, sf::Color color);
};

#endif // APP_HPP
