#include "includes/app.hpp"

PieceSprite::PieceSprite(Piece piece)
        : sf::CircleShape(TILE_SIZE / 2 - 5),  // Adjusting for a small border
          isWhitePiece(piece == Piece::whitePawn || piece == Piece::whiteKing),
          isKing(piece == Piece::whiteKing || piece == Piece::blackKing) {
    setFillColor(isWhitePiece ? sf::Color(200, 200, 200) : sf::Color::Black);
}

void PieceSprite::setPositionCentered(int x, int y) {
    sf::CircleShape::setPosition(x + (TILE_SIZE - getGlobalBounds().width) / 2, y + (TILE_SIZE - getGlobalBounds().height) / 2);
}

void App::drawBoard(const Board &board) {
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

void App::drawPieces(const Board &board) {
    // Draw the pieces
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 8; y++) {
            std::optional<Piece> piece = board.getAt(x, y);
            if (piece) {
                PieceSprite pieceSprite(*piece);
                pieceSprite.setPositionCentered((x * 2+(y+1)%2) * TILE_SIZE, y * TILE_SIZE);
                window.draw(pieceSprite);
            }
        }
    }
}

void App::drawWindow(const Game &game) {
    window.clear();
    drawBoard(game.getGameState().board);
    drawPieces(game.getGameState().board);
    window.display();
}

void App::gameLoop(Game &game) {
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        drawWindow(game);
    }
}

void App::launch() {
    window.create(sf::VideoMode(BOARD_DIMENSION, BOARD_DIMENSION), "Checkers");
    window.setFramerateLimit(60);
    Game game(GameState(Board(0xfff00000, 0xfff), true));
    gameLoop(game);
}
