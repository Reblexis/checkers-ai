// App.hpp

#ifndef APP_HPP
#define APP_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include "../../communication/includes/game.hpp" // Adjust the path as needed

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8 * TILE_SIZE;

class PieceSprite : public sf::CircleShape {
public:
    explicit PieceSprite(Piece piece);
    void setPositionCentered(int x, int y);
    bool isWhitePiece;
    bool isKing;
};

class App {
private:
    sf::RenderWindow window;

public:
    void drawBoard(Board &board);
    void drawPieces(Board &board);
    void drawWindow(Board &board);
    piece_move getMove(Board &board);
    void gameLoop();
    void launch();
};

#endif // APP_HPP

