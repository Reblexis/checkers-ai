#include "interface.hpp"

int evaluate(board b){
    return b.bcount() - b.wcount() * (b.next==0 ? -1 : 1);
}