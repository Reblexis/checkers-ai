#include <optional>


enum Piece{whitePawn, whiteKing, blackPawn, blackKing};



class Board{
    using bitboard = uint64_t; // First 32 bits represent all pieces, last 32 bits represent kings
    constexpr uint32_t NUM_SQUARES = 32;
    /*
     * The bits represent the board in the following way:
     * 0 1 2 3
     * 4 5 6 7 ...
     * .
     * .
     * .
     * For kings, all numbers are offset by 32
     */

    /*
     * By default, white is on the bottom of the board.
     */
private:
    bitboard whiteBitboard;
    bitboard blackBitboard;

public:
    Board(){
        reset();
    }

    optional<Piece> getAt(int x, int y){
        uint8_t pos = x + y*4;
        if(whiteBitboard&(1<<(NUM_SQUARES+pos))){
            return Piece::whiteKing;
        }
        if(blackBitboard&(1<<(NUM_SQUARES+pos))){
            return Piece::blackKing;
        }
        if(whiteBitboard&(1<<pos)){
            return Piece::whitePawn;
        }
        if(blackBitboard&(1<<pos)){
            return Piece::blackPawn;
        }
        return nullopt;
    }

    void reset(){
        whiteBitboard = 0x00000000fff00000; // The starting pawn setup
        blackBitboard = 0x0000000000000fff;
    }
};

class Game{
private:
    Board board;
    bool nextBlack = true;
public:
    Game(){
        reset();

    }
    void reset(){
        board.reset();
        nextBlack = true;
    }
};

Board::reset(){
    whiteBitboard =
}
