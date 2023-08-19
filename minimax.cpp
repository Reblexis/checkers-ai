#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"


// This is responsible for doing the minimax algorithm using the given interface
// for the game checkers

int evaluate(board &b){
    return (b.bcount()*- b.wcount()) * evalhyp[0]["pawn_value"] + (b.bkcount()* - b.wkcount())*evalhyp[0]["king_value"]; 
}

std::pair<int, move> search(board &b, int leftdepth, bool maximazing = true){
    move bestmove;
    if(leftdepth==0)
        return {evaluate(b), bestmove};
    int bestscore = 0;
    for(move nextmove: b.moves())
    {
        b.play(nextmove);

        std::pair<int, move> moveinfo = search(b, leftdepth-1);
        if((moveinfo.first<bestscore)^maximazing){
            bestscore = moveinfo.first;
            bestmove = nextmove;
        }

        b.undo(); 
    }

    return {bestscore, bestmove};
}

void findmove(board &b, int maxdepth = 6)
{
    search(b, 4);
}
