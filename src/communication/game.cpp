#include "includes/game.hpp"

Pos::Pos(int x, int y) : x(x), y(y) {}

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
    return index + 4 - ((index >> 2) & 1);
}
constexpr inline unsigned int br(unsigned int index){
    return index + 5 - ((index >> 2) & 1);
}
constexpr inline unsigned int king_row(unsigned int index){
    return index < 4;
}

void visualize_bitboard(bitboard_all bitboard){
    for(int i = 0; i<8; i++){
        for(int j = 0; j<4; j++){
            std::cout<<((bitboard>>(i*4+j))&1);
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

unsigned int Pos::indexFromPos() const{
    return x/2 + y*4;
}

Pos Pos::operator+(const Pos& other) const{
    return {x+other.x, y+other.y};
}

Pos Pos::operator-(const Pos& other) const{
    return {x-other.x, y-other.y};
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

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    if(piece == Piece::whitePawn)
        os<<"w";
    else if(piece == Piece::whiteKing)
    {
        os<<"W";
    }
    else if(piece == Piece::blackPawn)
        os<<"b";
    else if(piece == Piece::blackKing)
        os<<"B";
    else
        os<<"?";

    return os;
}

piece_move Move::getSubMove(unsigned int index) {
    // Return index-th sub-move
    if(index >= path.size()-1)
        throw std::runtime_error("Sub-move has to have 2 positions available. Index out of bounds.");
    unsigned int startPos = path[index].indexFromPos();
    if(rotated)
        startPos = 31-startPos;
    Direction direction = path[index].getDirection(path[index+1]);
    if(rotated)
        direction = path[index+1].getDirection(path[index]);
    return (static_cast<unsigned int>(direction) << 5) + startPos;
}

piece_move Move::getPieceMove() const {
    unsigned int startPos = path[0].indexFromPos();
    if(rotated)
        startPos = 31-startPos;
    piece_move pieceMove = startPos;
    for(unsigned int i = 1; i<path.size(); i++){
        Direction direction = path[i-1].getDirection(path[i]);
        if(rotated)
            direction = path[i].getDirection(path[i-1]);

        pieceMove |= (static_cast<unsigned int>(direction) << (i*3 + 2));
    }
    return pieceMove;
}

Board::Board(const bitboard_all whiteBitboard, const bitboard_all blackBitboard)
        : whiteBitboard(whiteBitboard), blackBitboard(blackBitboard) {}

Board getBoardFromStream(std::istream& is){
    std::vector<std::string> boardState(8);
    for(unsigned int i = 0; i < 8; i++) {
        for (unsigned int j = 0; j < 8; j++){
            char c;
            is>>c;
            boardState[i].push_back(c);
        }
    }

    bitboard_all whiteBitboard = 0;
    bitboard_all blackBitboard = 0;

    for(unsigned long long i = 0; i<32; i++)
    {
        Pos pos(i);
        whiteBitboard |= (long long)(tolower(boardState[pos.y][pos.x]) == 'w') << i;
        whiteBitboard |= (long long)(boardState[pos.y][pos.x] == 'W') << (i + 32);
        blackBitboard |= (long long)(tolower(boardState[pos.y][pos.x]) == 'b') << i;
        blackBitboard |= (long long)(boardState[pos.y][pos.x] == 'B') << (i + 32);
    }

    return {whiteBitboard, blackBitboard};
}

std::optional<Piece> Board::getAt(Pos piecePos) const
{
    // Compressed means that 0<x<4 and 0<y<8 meaning that we only consider the fields where any piece can even stand
    unsigned int x = piecePos.x;
    unsigned int y = piecePos.y;
    if((x+y)%2 == 0)
        return std::nullopt;
    x = x/2;

    if(x>3 || y>7)
        return std::nullopt;

    unsigned int pos = x + y*4;

    if (whiteBitboard & (1ll << (BOARD_SIZE + pos))) {
        return Piece::whiteKing;
    }
    if (blackBitboard & (1ll << (BOARD_SIZE + pos))) {
        return Piece::blackKing;
    }
    if (whiteBitboard & (1ll << pos)) {
        return Piece::whitePawn;
    }
    if (blackBitboard & (1ll << pos)) {
        return Piece::blackPawn;
    }
    return std::nullopt;
}

Board Board::getBoardRev() const {
    return {reverseBoard(blackBitboard), reverseBoard(whiteBitboard)};
}

int Board::allPiecesCount() const {
    return __builtin_popcount(whiteBitboard & 0xffffffff) + __builtin_popcount(blackBitboard & 0xffffffff);
}

int Board::whitePiecesCount() const {
    return __builtin_popcount(whiteBitboard & 0xffffffff);
}

int Board::blackPiecesCount() const {
    return __builtin_popcount(blackBitboard & 0xffffffff);
}

int Board::whiteKingsCount() const {
    return __builtin_popcount(whiteBitboard >> 32);
}

int Board::blackKingsCount() const {
    return __builtin_popcount(blackBitboard >> 32);
}

int Board::whitePawnsCount() const {
    return whitePiecesCount() - whiteKingsCount();
}

int Board::blackPawnsCount() const {
    return blackPiecesCount() - blackKingsCount();
}

bitboard Board::getWhitePieces() const {
    return whiteBitboard & 0xffffffff;
}

bitboard Board::getBlackPieces() const {
    return blackBitboard & 0xffffffff;
}

bitboard Board::getWhiteKings() const {
    return whiteBitboard >> 32;
}

bitboard Board::getBlackKings() const {
    return blackBitboard >> 32;
}

bitboard Board::reverseBitboard(const bitboard bitboardToReverse) {
    bitboard reversedBitboard = bitboardToReverse;
    reversedBitboard = (reversedBitboard & 0x55555555) << 1 | (reversedBitboard & 0xAAAAAAAA) >> 1;
    reversedBitboard = (reversedBitboard & 0x33333333) << 2 | (reversedBitboard & 0xCCCCCCCC) >> 2;
    reversedBitboard = (reversedBitboard & 0x0F0F0F0F) << 4 | (reversedBitboard & 0xF0F0F0F0) >> 4;
    reversedBitboard = (reversedBitboard & 0x00FF00FF) << 8 | (reversedBitboard & 0xFF00FF00) >> 8;
    reversedBitboard = (reversedBitboard & 0x0000FFFF) << 16 | (reversedBitboard & 0xFFFF0000) >> 16;
    return reversedBitboard;
}

bitboard_all Board::reverseBoard(const bitboard_all boardToReverse) {
    bitboard pieces = reverseBitboard(boardToReverse & 0xffffffff);
    bitboard kings = reverseBitboard(boardToReverse >> 32);
    return (((bitboard_all)kings) << 32) | pieces;
}

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
    os<<"("<<pos.x<<", "<<pos.y<<")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Board& board)
{
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            if(board.getAt(Pos(j, i)))
                os<<board.getAt(Pos(j, i)).value()<<" ";
            else
                os<<". ";
        }
        os<<std::endl;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move){
    os<<"Move: ";
    for(auto pos: move.path){
        os<<pos<<" ";
    }
    return os;
}

GameState::GameState(Board board, bool nextBlack)
        : board(board), nextBlack(nextBlack) {
    calculateAvailableMoves();
}

Board GameState::getPerspectiveBoard() const {
    return nextBlack ? board.getBoardRev() : board;
}

std::span<const piece_move> GameState::getAvailableMoves() const {
    return {availableMoves};
}

Move GameState::getMove(piece_move pieceMove) const {
    unsigned int currentPos = pieceMove & 0x1f;

    Move move{pieceMove, {Pos(currentPos)}, nextBlack};

    pieceMove >>= 5;
    Board perspectiveBoard = getPerspectiveBoard();
    bitboard_all controlBitboard = perspectiveBoard.whiteBitboard;
    bitboard_all enemyBitboard = perspectiveBoard.blackBitboard;

    while (pieceMove) { // TODO: MERGE THIS WITH MAKE_MOVE
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
            if(enemyBitboard & (1ll<<currentPos))
                enemyBitboard &= (~((1ll<<currentPos) | (1ll<<(currentPos+32))));
            else
                break;
        }

        move.path.emplace_back(currentPos);

        pieceMove >>= 3;
    }

    if(nextBlack){
        // Adjust for board orientation
        for(auto& pos : move.path){
            pos = Pos(31-pos.indexFromPos());
        }
    }

    return move;
}

std::vector<Move> GameState::getAvailableMoves2() const {
    std::vector<Move> moves;
    for (auto pieceMove: availableMoves)
    {
        Move move = getMove(pieceMove);

        moves.push_back(move);
    }
    return moves;
}

void GameState::calculateAvailableMoves() {
    availableMoves.clear();

    Board boards = getPerspectiveBoard();
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
        if(lastPos > 7 && lastPos % 4 != 0 && (curEnemyPieces&(1<<(tl(lastPos))) && (anyPieces&(1<<(tl(tl(lastPos))))) == 0)){
            calculateMoves(currentMove | (1<<(jumpCount*3 + 5)), jumpCount+1, tl(tl(lastPos)), curEnemyPieces^(1<<(tl(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump top-right
        if(lastPos > 7 && lastPos % 4 != 3 && (curEnemyPieces&(1<<(tr(lastPos))) && (anyPieces&(1<<(tr(tr(lastPos))))) == 0)){
            calculateMoves(currentMove | (2<<(jumpCount*3 + 5)), jumpCount+1, tr(tr(lastPos)), curEnemyPieces^(1<<(tr(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump bottom-left
        if(isKing && lastPos < 24 && lastPos % 4 != 0 && (curEnemyPieces&(1<<(bl(lastPos))) && (anyPieces&(1<<bl(bl(lastPos)))) == 0)){
            calculateMoves(currentMove | (3<<(jumpCount*3 + 5)), jumpCount+1, bl(bl(lastPos)), curEnemyPieces^(1<<(bl(lastPos))), isKing);
            anyJumps = true;
        }
        // Jump bottom-right
        if(isKing && lastPos < 24 && lastPos % 4 != 3 && (curEnemyPieces&(1<<br(lastPos)) && (anyPieces&(1<<br(br(lastPos)))) == 0)){
            calculateMoves(currentMove | (4<<(jumpCount*3 + 5)), jumpCount+1, br(br(lastPos)), curEnemyPieces^(1<<br(lastPos)), isKing);
            anyJumps = true;
        }

        if(!anyJumps&&jumpCount>0){
            availableMoves.push_back(currentMove);
        }
    };

    for(int i = 0; i < BOARD_SIZE; i++){
        if(controlKings&(1<<i))
        {
            calculateMoves(i, 0, i, enemyPieces, true);
        }
    }
    if(!availableMoves.empty())
        return;

    bitboard controlPawns = controlPieces^controlKings;
    for(int i = 0; i < BOARD_SIZE; i++){
        if((controlPawns)&(1<<i))
        {
            calculateMoves(i, 0, i, enemyPieces, false);
        }
    }
    if(!availableMoves.empty())
        return;

    bitboard anyPieces = controlPieces | enemyPieces;
    for(int i = 0; i < BOARD_SIZE; i++){
        if((controlPieces&(1<<i))==0)
            continue;
        // Move top-left
        if(i>3 && i%8!=4 && (anyPieces&(1<<tl(i))) == 0)
        {
            availableMoves.push_back(i | (Direction::topLeft<<5));
        }
        // Move top-right
        if(i>3 && i%8!=3 && (anyPieces&(1<<tr(i))) == 0)
        {
            availableMoves.push_back(i | (Direction::topRight<<5));
        }

        if(controlKings&(1<<i))
        {
            // Move bottom-left
            if(i<28 && i%8!=4 && (anyPieces&(1<<bl(i))) == 0)
            {
                availableMoves.push_back(i | (Direction::bottomLeft<<5));
            }
            // Move bottom-right
            if(i<28 && i%8!=3 && (anyPieces&(1<<br(i))) == 0)
            {
                availableMoves.push_back(i | (Direction::bottomRight<<5));
            }
        }
    }
}

Game::Game(){
    addGameState(GameState(Board(0xfff00000, 0xfff), true));
}

Game::Game(const GameState& state) {
    addGameState(state);
}

void Game::addGameState(const GameState& state) {
    gameHistory.push_back(state);
}

void Game::undoMove() {
    if (gameHistory.size() <= 1) {
        throw std::runtime_error("Cannot undo move. No moves have been made.");
    }
    gameHistory.pop_back();
}

void Game::reset(const GameState& state) {
    gameHistory.clear();
    addGameState(state);
}

const GameState& Game::getGameState() const {
    return gameHistory.back();
}

std::ostream& operator<<(std::ostream& os, const GameState& gameState) {
    os << (gameState.nextBlack ? "black" : "white") << '\n';
    os<<gameState.board;
    return os;
}

void Game::makeMove(piece_move pieceMove, bool final) {
    // Final indicates whether the sub-move is the last one of the whole move (with multiple jumps there is only one final sub-move)

#if CHECK_VALID_MOVES
    if (std::find(gameHistory.back().getAvailableMoves().begin(), gameHistory.back().getAvailableMoves().end(), pieceMove) == gameHistory.back().getAvailableMoves().end()) {
        throw std::runtime_error("Invalid move. Move not in available moves.");
    }
#endif
    if (pieceMove == 0) {
        throw std::runtime_error("Invalid move. Move is 0.");
    }

    unsigned int currentPos = pieceMove & 0x1f;

    pieceMove >>= 5;
    Board perspectiveBoard = gameHistory.back().getPerspectiveBoard();
    bitboard_all controlBitboard = perspectiveBoard.whiteBitboard;
    bitboard_all enemyBitboard = perspectiveBoard.blackBitboard;
    bool isKing = controlBitboard & (1ll << (currentPos + 32));

    while (pieceMove) {
        unsigned int direction = static_cast<Direction>(pieceMove & 0x7);
        controlBitboard &= (~(1ll << currentPos)) & (~(1ll << (currentPos + 32)));
        for(int i = 0; i<2; i++){
            if (direction == Direction::topLeft)
                currentPos = tl(currentPos);
            else if (direction == Direction::topRight)
                currentPos = tr(currentPos);
            else if (direction == Direction::bottomLeft)
                currentPos = bl(currentPos);
            else if (direction == Direction::bottomRight)
                currentPos = br(currentPos);
            else
                throw std::runtime_error("Invalid direction.");

            if(enemyBitboard & (1ll<<currentPos))
                enemyBitboard &= (~((1ll<<currentPos) | (1ll<<(currentPos+32))));
            else
                break;
        }

        if (king_row(currentPos))
            isKing = true;

        pieceMove >>= 3;

        controlBitboard |= (1ll << currentPos);
        if (isKing)
            controlBitboard |= (1ll << (currentPos + 32));
    }

    Board newBoard(controlBitboard, enemyBitboard);

    const GameState newGameState(gameHistory.back().nextBlack ? newBoard.getBoardRev() : newBoard,
                                 gameHistory.back().nextBlack^final);
    addGameState(newGameState);
}

bool Game::isFinished() const {
    return gameHistory.back().getAvailableMoves().empty();
}
