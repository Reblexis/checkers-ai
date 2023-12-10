#include "includes/app.hpp"

PieceSprite::PieceSprite(Piece piece)
        : sf::CircleShape((float)TILE_SIZE / 2.3f),  // Adjusting for a small border
          isWhitePiece(piece == Piece::whitePawn || piece == Piece::whiteKing),
          isKing(piece == Piece::whiteKing || piece == Piece::blackKing) {
    setFillColor(isWhitePiece ? WHITE_PIECE_COLOR : BLACK_PIECE_COLOR);
    if(isKing)
    {
        setOutlineThickness(-5);
        setOutlineColor(sf::Color::Yellow);
    }
}

void PieceSprite::setPositionCentered(int x, int y) {
    sf::CircleShape::setPosition((float)x + ((float)TILE_SIZE - getGlobalBounds().width) / 2, (float)y + ((float)TILE_SIZE - getGlobalBounds().height) / 2);
}

void App::drawBoard(const Board &board) {
    // Draw the checkerboard
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setPosition(i * TILE_SIZE, j * TILE_SIZE);
            tile.setFillColor((i + j) % 2 == 0 ? WHITE_TILE_COLOR : BLACK_TILE_COLOR);  // Beige and Sienna
            window.draw(tile);
        }
    }
}

void App::drawPieces(const Board &board) {
    // Draw the pieces
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            std::optional<Piece> piece = board.getAt(Pos(x, y));
            if (piece) {
                PieceSprite pieceSprite(*piece);
                pieceSprite.setPositionCentered(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(pieceSprite);
            }
        }
    }
}

void App::highlightField(Pos pos, sf::Color color)
{
    sf::RectangleShape selectedSquare(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    selectedSquare.setPosition(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
    selectedSquare.setFillColor(color);
    window.draw(selectedSquare);
}

void App::drawUI(const UI &ui) {
    if (ui.selectedSquare) {
        highlightField(*(ui.selectedSquare), SELECTED_TILE_COLOR);

        for(Pos pos: ui.possibleMoves)
        {
            highlightField(pos, POSSIBLE_MOVE_COLOR);
        }
    }
}

void App::drawWindow(const Game &game, const UI &ui){
    window.clear();
    drawBoard(game.getGameState().board);
    drawUI(ui);
    drawPieces(game.getGameState().board);
    window.display();
}

void App::gameLoop(Game &game, std::optional<Agent> agent1, std::optional<Agent> agent2, UI &ui) {
    std::vector<Move> possibleMoves;
    int currentSubMove = 1;
    bool newMove = true;
    bool mousePressed = false;

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
            if(newMove) {
                possibleMoves = game.getGameState().getAvailableMoves2();
                currentSubMove = 1;
                newMove = false;
            }
            /*
             *Human-controlled
             * Allows the human to select a piece and then select a destination for it. If there is forced jump it forces the human to select the next move.
             */

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)&&!mousePressed&&window.hasFocus()){
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                Pos highlightedPiecePos(mousePosition.x / TILE_SIZE, mousePosition.y / TILE_SIZE);

                bool isMove = false;
                for (auto move : ui.possibleMoves)
                {
                    if (move == highlightedPiecePos)
                    {
                        isMove = true;
                    }
                }
                if(isMove)
                {
                    std::vector<Move> newPossibilities;
                    std::optional<Move> madeMove;
                    ui.possibleMoves.clear();
                    for(auto move : possibleMoves)
                    {
                        if(move.path[currentSubMove] == highlightedPiecePos && (move.path[currentSubMove-1] == ui.selectedSquare || currentSubMove > 1))
                        {
                            madeMove = move;
                            if(move.path.size() > currentSubMove + 1)
                            {
                                newPossibilities.emplace_back(move);
                                ui.possibleMoves.push_back(move.path[currentSubMove+1]);
                            }
                        }
                    }
                    if(!madeMove)
                        throw std::runtime_error("No fitting move found!");

                    game.makeMove(madeMove->getSubMove(currentSubMove-1),  newPossibilities.empty());
                    possibleMoves = newPossibilities;
                    if(!possibleMoves.empty())
                    {
                        currentSubMove++;
                    }
                    else{
                        newMove = true;
                        ui.selectedSquare = std::nullopt;
                    }
                }
                else if(currentSubMove == 1){
                    std::optional <Piece> piece = game.getGameState().board.getAt(highlightedPiecePos);

                    if (piece)
                    {
                        ui.selectedSquare = highlightedPiecePos;
                        ui.possibleMoves.clear();
                        for(auto move : possibleMoves)
                        {
                            if(move.path[0] == highlightedPiecePos)
                            {
                                ui.possibleMoves.push_back(move.path[1]);
                            }
                        }
                    }
                    else
                    {
                        ui.possibleMoves.clear();
                        ui.selectedSquare = std::nullopt;
                    }
                }
           }
            mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        }

        drawWindow(game, ui);
    }
}

void App::launch() {
    window.create(sf::VideoMode(BOARD_DIMENSION, BOARD_DIMENSION), "Checkers");
    window.setFramerateLimit(60);
    Game game(GameState(Board(0xfff00000, 0xfff), true));
    UI ui;
    Agent agent1(std::filesystem::path("../data/hyperparameters1.json"));
    gameLoop(game, agent1, std::nullopt, ui);
}
