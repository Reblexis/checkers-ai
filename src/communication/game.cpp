#include "includes/game.hpp"

Pos::Pos(unsigned int x, unsigned int y) : x(x), y(y) {}

Pos::Pos(unsigned int index){
    x = (index % 4) * 2;
    y = index / 4;
    x += ((y % 2)==0);
}

// Return neighboring cells
constexpr inline unsigned int tl(unsigned int index){
    return index - 4 - ((index >> 2) & 1);
}
constexpr inline unsigned int tr(unsigned int index){
    return index - 3 - ((index >> 2) & 1);
}
constexpr inline unsigned int bl(unsigned int index){
    return index + 4 - (index >> 2 & 1);
}
constexpr inline unsigned int br(unsigned int index){
    return index + 5 - ((index >> 2 & 1));
}
constexpr inline unsigned int king_row(unsigned int index){
    return index < 4;
}

unsigned int Pos::indexFromPos() const{
    return x/2 + y*4;
}

Pos Pos::operator+(const Pos& other) const{
    return Pos(x+other.x, y+other.y);
}

Pos Pos::operator-(const Pos& other) const{
    return Pos(x-other.x, y-other.y);
}

Direction Pos::getDirection(Pos pos) const{
    Pos diff = pos - *this;
    if (diff.x == diff.y && diff.x < 0)
        return Direction::topLeft;
    else if (diff.x == -diff.y && diff.x > 0)
        return Direction::topRight;
    else if (diff.x == -diff.y && diff.x < 0)
        return Direction::bottomLeft;
    else if (diff.x == diff.y && diff.x > 0)
        return Direction::bottomRight;
    else
        throw std::runtime_error("Invalid direction.");
}

bool Pos::operator==(const Pos& other) const{
    return x == other.x && y == other.y;
}

piece_move Move::getSubMove(unsigned int index) {
    // Return index-th sub-move
    if(index >= path.size()-1)
        throw std::runtime_error("Sub-move has to have 2 positions available. Index out of bounds.");
    unsigned int startPos = path[index].indexFromPos();
    Direction direction = path[index].getDirection(path[index+1]);
    return (static_cast<unsigned int>(direction) << 5) + startPos;
}

Board::Board(const bitboard_all whiteBitboard, const bitboard_all blackBitboard)
        : whiteBitboard(whiteBitboard), blackBitboard(blackBitboard) {}

const std::optional<Piece> Board::getAt(Pos piecePos) const {
    // Compressed means that 0<x<4 and 0<y<8 meaning that we only consider the fields where any piece can even stand
    int x = piecePos.x;
    int y = piecePos.y;
    if((x+y)%2 == 0)
        return std::nullopt;
    x = x/2;

    if(x<0 || x>3 || y<0 || y>7)
        return std::nullopt;

    uint8_t pos = x + y * 4;
    if (whiteBitboard & (1 << (BOARD_SIZE + pos))) {
        return Piece::whiteKing;
    }
    if (blackBitboard & (1 << (BOARD_SIZE + pos))) {
        return Piece::blackKing;
    }
    if (whiteBitboard & (1 << pos)) {
        return Piece::whitePawn;
    }
    if (blackBitboard & (1 << pos)) {
        return Piece::blackPawn;
    }
    return std::nullopt;
}

const Board Board::getBoardRev() const {
    return Board(reverseBoard(blackBitboard), reverseBoard(whiteBitboard));
}

const int Board::whitePiecesCount() const {
    return __builtin_popcount(whiteBitboard & 0xffffffff);
}

const int Board::blackPiecesCount() const {
    return __builtin_popcount(blackBitboard & 0xffffffff);
}

const int Board::whiteKingsCount() const {
    return __builtin_popcount(whiteBitboard >> 32);
}

const int Board::blackKingsCount() const {
    return __builtin_popcount(blackBitboard >> 32);
}

const int Board::whitePawnsCount() const {
    return whitePiecesCount() - whiteKingsCount();
}

const int Board::blackPawnsCount() const {
    return blackPiecesCount() - blackKingsCount();
}

const bitboard Board::getWhitePieces() const {
    return whiteBitboard & 0xffffffff;
}

const bitboard Board::getBlackPieces() const {
    return blackBitboard & 0xffffffff;
}

const bitboard Board::getWhiteKings() const {
    return whiteBitboard >> 32;
}

const bitboard Board::getBlackKings() const {
    return blackBitboard >> 32;
}

const bitboard Board::reverseBitboard(const bitboard bitboardToReverse) const {
    bitboard reversedBitboard = bitboardToReverse;
    reversedBitboard = (reversedBitboard & 0x55555555) << 1 | (reversedBitboard & 0xAAAAAAAA) >> 1;
    reversedBitboard = (reversedBitboard & 0x33333333) << 2 | (reversedBitboard & 0xCCCCCCCC) >> 2;
    reversedBitboard = (reversedBitboard & 0x0F0F0F0F) << 4 | (reversedBitboard & 0xF0F0F0F0) >> 4;
    reversedBitboard = (reversedBitboard & 0x00FF00FF) << 8 | (reversedBitboard & 0xFF00FF00) >> 8;
    reversedBitboard = (reversedBitboard & 0x0000FFFF) << 16 | (reversedBitboard & 0xFFFF0000) >> 16;
    return reversedBitboard;
}

const bitboard_all Board::reverseBoard(const bitboard_all boardToReverse) const {
    bitboard pieces = reverseBitboard(boardToReverse & 0xffffffff);
    bitboard kings = reverseBitboard(boardToReverse >> 32);
    return (((bitboard_all)kings) << 32) | pieces;
}


GameState::GameState(Board board, bool nextBlack)
        : board(board), nextBlack(nextBlack), hash(0) {
    calculateAvailableMoves();
}

std::span<const piece_move> GameState::getAvailableMoves() const {
    return std::span<const piece_move>(availableMoves);
}

std::vector<Move> GameState::getAvailableMoves2() const {
    std::vector<Move> moves;
    for (auto pieceMove: availableMoves)
    {
        unsigned int currentPos = pieceMove & 0x1f;

        Move move{pieceMove, {Pos(currentPos)}};

        pieceMove >>= 5;
        Board perspectiveBoard = nextBlack ? board.getBoardRev(): board;
        bitboard_all controlBitboard = perspectiveBoard.whiteBitboard;
        bitboard_all enemyBitboard = perspectiveBoard.blackBitboard;
        bool isKing = controlBitboard & (1 << (currentPos + 32));

        while (pieceMove) {
            unsigned int direction = pieceMove & 0x7;
            unsigned int startPos = currentPos;
            for(int i = 0; i<2; i++){
                if (direction == Direction::topLeft)
                    currentPos = tl(currentPos);
                else if (direction == Direction::topRight)
                    currentPos = tr(currentPos);
                else if (direction == Direction::bottomLeft)
                    currentPos = bl(currentPos);
                else
                    currentPos = br(currentPos);
                if(enemyBitboard & (1<<currentPos)==0)
                    break;
            }

            move.path.push_back(Pos(currentPos));
            std::cout<<"second pos: "<< currentPos<<'\n';

            if (king_row(currentPos))
                isKing = true;

            pieceMove >>= 3;
        }

        moves.push_back(move);
    }
    return moves;
}

void GameState::calculateAvailableMoves() {
    availableMoves.clear();
    std::cout<<"Calculating available moves!"<<std::endl;

    Board boards = nextBlack ? board.getBoardRev() : board;
    bitboard_all controlBitboard = boards.whiteBitboard; // The bitboard_all of the pieces that can move next
    bitboard_all enemyBitboard = boards.blackBitboard; // The bitboard_all of the pieces that can be captured

    bitboard controlPieces = controlBitboard&0xffffffff;
    bitboard controlKings = controlBitboard>>32;
    bitboard enemyPieces = enemyBitboard&0xffffffff;

    std::function<void(piece_move, unsigned int, position, bitboard, bool)> calculateMoves = [&](piece_move currentMove, unsigned int jumpCount, position lastPos, bitboard curEnemyPieces, bool isKing){
        bool anyJumps = false;
        isKing |= king_row(lastPos);
        bitboard anyPieces = controlPieces | curEnemyPieces;
        // Jump top-left
        if(lastPos > 7 && lastPos % 4 != 0 && (curEnemyPieces&(1<<(tl(lastPos))) && !(anyPieces&(1<<(tl(tl(lastPos))))))){
            calculateMoves(currentMove | (1<<(jumpCount*3 + 5)), jumpCount+1, tl(tl(lastPos)), curEnemyPieces^(1<<(tl(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump top-right
        if(lastPos > 7 && lastPos % 4 != 3 && (curEnemyPieces&(1<<(tr(lastPos))) && !(anyPieces&(1<<(tr(tr(lastPos))))))){
            calculateMoves(currentMove | (2<<(jumpCount*3 + 5)), jumpCount+1, tr(tr(lastPos)), curEnemyPieces^(1<<(tr(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump bottom-left
        if(isKing && lastPos < 24 && lastPos % 4 != 0 && (curEnemyPieces&(1<<(bl(lastPos))) && !(anyPieces&(1<<bl(bl(lastPos)))))){
            calculateMoves(currentMove | (3<<(jumpCount*3 + 5)), jumpCount+1, bl(bl(lastPos)), curEnemyPieces^(1<<(bl(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump bottom-right
        if(isKing && lastPos < 24 && lastPos % 4 != 3 && (curEnemyPieces&(1<<br(lastPos)) && !(anyPieces&(1<<br(br(lastPos)))))){
            calculateMoves(currentMove | (4<<(jumpCount*3 + 5)), jumpCount+1, br(br(lastPos)), curEnemyPieces^(1<<br(lastPos)), isKing);
            anyJumps = true;
        }

        if(!anyJumps){
            availableMoves.push_back(currentMove);
        }
    };

    for(int i = 0; i < BOARD_SIZE; i++){
        if(controlKings&(1<<i))
        {
            calculateMoves(i, 0, i, enemyPieces, true);
        }
    }
    std::cout<<"Available moves: "<<availableMoves.size()<<std::endl;
    if(availableMoves.size() > 0)
        return;

    bitboard controlPawns = controlPieces^controlKings;
    for(int i = 0; i < BOARD_SIZE; i++){
        if((controlPawns)&(1<<i))
        {
            calculateMoves(i, 0, i, enemyPieces, false);
        }
    }
    std::cout<<"Available moves: "<<availableMoves.size()<<std::endl;
    if(availableMoves.size() > 0)
        return;

    bitboard anyPieces = controlPieces | enemyPieces;
    for(int i = 0; i < BOARD_SIZE; i++){
        if(!controlPieces&(1<<i))
            continue;

        unsigned int sub = (i >> 2) & 1;
        // Move top-left
        if(i>3 && i%8!=4 && !(anyPieces&(1<<(i-4-sub))))
        {
            availableMoves.push_back(i | (1<<5));
        }
        // Move top-right
        if(i>3 && i%8!=3 && !(anyPieces&(1<<(i-3-sub))))
        {
            availableMoves.push_back(i | (2<<5));
        }

        if(controlKings&(1<<i))
        {
            // Move bottom-left
            if(i<28 && i%8!=4 && !(anyPieces&(1<<(i+4-sub))))
            {
                availableMoves.push_back(i | (3<<5));
            }
            // Move bottom-right
            if(i<28 && i%8!=3 && !(anyPieces&(1<<(i+5-sub))))
            {
                availableMoves.push_back(i | (4<<5));
            }
        }
    }
    std::cout<<"Available moves: "<<availableMoves.size()<<std::endl;
}

Game::Game(const GameState state) {
    addGameState(state);
}

void Game::addGameState(const GameState state) {
    gameHistory.push_back(state);
}

void Game::undoMove() {
    if (gameHistory.size() <= 1) {
        throw std::runtime_error("Cannot undo move. No moves have been made.");
    }
    gameHistory.pop_back();
}

void Game::reset(GameState state) {
    gameHistory.clear();
    addGameState(state);
}

const GameState& Game::getGameState() const {
    return gameHistory.back();
}

void Game::makeMove(piece_move pieceMove) {
    unsigned int currentPos = pieceMove & 0x1f;

    pieceMove >>= 5;
    Board perspectiveBoard = gameHistory.back().nextBlack ? gameHistory.back().board.getBoardRev()
                                                          : gameHistory.back().board;
    bitboard_all controlBitboard = perspectiveBoard.whiteBitboard;
    bitboard_all enemyBitboard = perspectiveBoard.blackBitboard;
    bool isKing = controlBitboard & (1 << (currentPos + 32));
    controlBitboard &= (~(1 << currentPos)) & (~(1 << (currentPos + 32)));

    while (pieceMove) {
        unsigned int direction = pieceMove & 0x7;
        unsigned int startPos = currentPos;
        for(int i = 0; i<2; i++){
            if (direction == Direction::topLeft)
                currentPos = tl(currentPos);
            else if (direction == Direction::topRight)
                currentPos = tr(currentPos);
            else if (direction == Direction::bottomLeft)
                currentPos = bl(currentPos);
            else
                currentPos = br(currentPos);

#if CHECK_VALID_MOVES
            if(currentPos>31 || currentPos<0 || controlBitboard&(1<<currentPos) || (direction>=3 && !isKing))
                throw std::runtime_error("Invalid move.");
#endif
            if(enemyBitboard & (1<<currentPos)==0)
                break;
        }

        // Do a jump if standing on an enemy piece
        if (enemyBitboard & (1 << currentPos)) {
            enemyBitboard &= (~(1 << currentPos)) & (~(1 << (currentPos + 32)));
            currentPos += (currentPos - startPos);
        }

#if CHECK_VALID_MOVES
        if(currentPos>31 || currentPos<0 || (controlBitboard|enemyBitboard)&(1<<currentPos))
            throw std::runtime_error("Invalid move. Incorrect jump.");
#endif

        if (king_row(currentPos))
            isKing = true;

        pieceMove >>= 3;
    }

    controlBitboard |= (1 << currentPos);
    if (isKing)
        controlBitboard |= (1 << (currentPos + 32));

    Board newBoard(controlBitboard, enemyBitboard);

    const GameState newGameState(gameHistory.back().nextBlack ? newBoard : newBoard.getBoardRev(),
                                 !gameHistory.back().nextBlack);
    addGameState(newGameState);
}