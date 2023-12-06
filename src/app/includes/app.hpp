#ifndef APP_HPP
#define APP_HPP

#include <SFML/Graphics.hpp>
#include "../../communication/includes/game.hpp"
#include <iostream>
#include <optional>

const unsigned int TILE_SIZE = 80;
const unsigned int BOARD_DIMENSION = 8 * TILE_SIZE;

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
    void drawWindow(const Game &game);
    void gameLoop(Game& game);
    void launch();

private:
    sf::RenderWindow window;
};

#endif // APP_HPP
