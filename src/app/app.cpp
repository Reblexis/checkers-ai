#include <SFML/Graphics.hpp>

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8 * TILE_SIZE;

class CheckersPiece : public sf::CircleShape {
public:
    CheckersPiece(bool isPlayerPiece)
            : sf::CircleShape(TILE_SIZE / 2 - 5),  // Adjusting for a small border
              isPlayerPiece(isPlayerPiece) {
        setFillColor(isPlayerPiece ? sf::Color(255, 0, 0) : sf::Color::Black);
    }

    void setPositionCentered(int x, int y) {
        sf::CircleShape::setPosition(x + (TILE_SIZE - getGlobalBounds().width) / 2, y + (TILE_SIZE - getGlobalBounds().height) / 2);
    }

    bool isPlayerPiece;
};

void launchApp()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE, BOARD_SIZE), "Checkers");

    std::vector<CheckersPiece> pieces;

    // Setup the Board
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2 == 1) {
                // Add pieces to starting positions
                if (j < 3) {
                    pieces.emplace_back(false);
                    pieces.back().setPositionCentered(i * TILE_SIZE, j * TILE_SIZE);
                } else if (j > 4) {
                    pieces.emplace_back(true);
                    pieces.back().setPositionCentered(i * TILE_SIZE, j * TILE_SIZE);
                }
            }
        }
    }

    bool pieceSelected = false;
    CheckersPiece* selectedPiece = nullptr;

    // Main game loop
    for(int i = 0; i < 10000000; i++) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    for (CheckersPiece &piece : pieces) {
                        if (piece.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && piece.isPlayerPiece) {
                            pieceSelected = true;
                            selectedPiece = &piece;
                            break;
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    pieceSelected = false;
                    if (selectedPiece) {
                        // Set the centered position
                        selectedPiece->setPositionCentered((event.mouseButton.x / TILE_SIZE) * TILE_SIZE, (event.mouseButton.y / TILE_SIZE) * TILE_SIZE);
                        selectedPiece = nullptr;
                    }
                }
            }
        }

        window.clear();
        // Draw the checkerboard
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                tile.setPosition(i * TILE_SIZE, j * TILE_SIZE);
                tile.setFillColor((i + j) % 2 == 0 ? sf::Color(245, 245, 220) : sf::Color(160, 82, 45));  // Beige and Sienna
                window.draw(tile);
            }
        }

        // Draw the pieces
        for (CheckersPiece &piece : pieces) {
            window.draw(piece);
        }

        window.display();
    }
}
