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

struct Pos {
    int x;
    int y;
};

class UI{
public:
    std::optional<Pos> selectedSquare;
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
    void gameLoop(Game& game, std::optional<Agent> agent1, std::optional<Agent> agent2, UI &ui);
    void launch();

private:
    sf::RenderWindow window;
};

#endif // APP_HPP
