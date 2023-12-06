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

void App::drawUI(const UI &ui) {
    if (ui.selectedSquare) {
        sf::RectangleShape selectedSquare(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        selectedSquare.setPosition(ui.selectedSquare->x * TILE_SIZE, ui.selectedSquare->y * TILE_SIZE);
        selectedSquare.setFillColor(sf::Color(255, 112, 10, 70));
        window.draw(selectedSquare);
    }
}

void App::drawWindow(const Game &game, const UI &ui){
    window.clear();
    drawBoard(game.getGameState().board);
    drawPieces(game.getGameState().board);
    drawUI(ui);
    window.display();
}

void App::gameLoop(Game &game, std::optional<Agent> agent1, std::optional<Agent> agent2, UI &ui) {
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (agent1 && game.getGameState().nextBlack) {
            std::pair<int, piece_move> bestMove = agent1->findBestMove(game);
            game.makeMove(bestMove.second);
        } else if (agent2 && !game.getGameState().nextBlack) {
            std::pair<int, piece_move> bestMove = agent2->findBestMove(game);
            game.makeMove(bestMove.second);
        }
        else {
            /*
             *Human-controlled
             * Allows the human to select a piece and then select a destination for it. If there is forced jump it forces the human to select the next move.
             */
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                int x = mousePosition.x / TILE_SIZE;
                int y = mousePosition.y / TILE_SIZE;
                std::optional <Piece> piece = game.getGameState().board.getAt(x, y, false);

                if (piece)
                {
                    ui.selectedSquare = Pos{x, y};
                    message("Selected square: " + std::to_string(x) + ", " + std::to_string(y));
                }
                else
                {
                    message("No piece selected");
                    ui.selectedSquare = std::nullopt;
                }
            }
        }

        drawWindow(game, ui);
    }
}

void App::launch() {
    window.create(sf::VideoMode(BOARD_DIMENSION, BOARD_DIMENSION), "Checkers");
    window.setFramerateLimit(60);
    Game game(GameState(Board(0xfff00000, 0xfff), true));
    UI ui;
    gameLoop(game, std::nullopt, std::nullopt, ui);
}
