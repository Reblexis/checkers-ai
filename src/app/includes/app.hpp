#ifndef APP_HPP
#define APP_HPP

#include <SFML/Graphics.hpp>

#include "../../communication/includes/game.hpp"

class App {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;

    void drawBoard(Board &board);
    void drawPieces(Board &board);
    void drawWindow(Board &board);

public:
    void launch();
    move getMove(Board &board);
};;

#endif
