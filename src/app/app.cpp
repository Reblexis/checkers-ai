#include <SFML/Graphics.hpp>

#include "includes/app.hpp"

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8 * TILE_SIZE;

class CheckersPiece : public sf::CircleShape {
public:
    CheckersPiece(bool isWhitePiece)
            : sf::CircleShape(TILE_SIZE / 2 - 5),  // Adjusting for a small border
              isWhitePiece(isWhitePiece) {
        setFillColor(isWhitePiece ? sf::Color(200, 200, 200) : sf::Color::Black);
    }

    void setPositionCentered(int x, int y) {
        sf::CircleShape::setPosition(x + (TILE_SIZE - getGlobalBounds().width) / 2, y + (TILE_SIZE - getGlobalBounds().height) / 2);
    }

    bool isWhitePiece;
};

void App::drawBoard(Board &board) {
    // Draw the checkerboard
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setPosition(i * TILE_SIZE, j * TILE_SIZE);
            tile.setFillColor((i + j) % 2 == 0 ? sf::Color(245, 245, 220) : sf::Color(160, 82, 45));  // Beige and Sienna
            window.draw(tile);
        }
    }
}

void App::drawPieces(Board &board) {
    // Draw the pieces
    for(int i = 0; i<8; i++) {
        for (int j = 0; j < 8; j++) {
            int index = i * 8 + j;
            if ((board.blackPieces | board.whitePieces) & (1 << index)){
                CheckersPiece piece((board.whitePieces) & (1 << index));
                piece.setPositionCentered(i * TILE_SIZE, j * TILE_SIZE);
                window.draw(piece);
            }
        }
    }
}

void App::drawWindow(Board &board) {
    window.clear();
    drawBoard(board);
    drawPieces(board);
    window.display();
}

move App::getMove(Board &board) {
    drawWindow(board);

    bool pieceSelected = false;
    CheckersPiece* selectedPiece = nullptr;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    for (int i = 0; i<8; i++) {
                        for (int j = 0; j < 8; j++) {
                            int index = i * 8 + j;
                            if ((board.blackPieces | board.whitePieces) & (1 << index)){
                                CheckersPiece piece((board.whitePieces) & (1 << index));
                                piece.setPositionCentered(i * TILE_SIZE, j * TILE_SIZE);
                                if (piece.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && piece.isWhitePiece) {
                                    pieceSelected = true;
                                    selectedPiece = &piece;
                                    break;
                                }
                            }
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
    }
}

void App::launch() {
    window.create(sf::VideoMode(BOARD_SIZE, BOARD_SIZE), "Checkers");
    window.setFramerateLimit(60);
}

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
                        if (piece.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && piece.isWhitePiece) {
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
