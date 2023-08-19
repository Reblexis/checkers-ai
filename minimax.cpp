#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"


// This is responsible for doing the minimax algorithm using the given interface
// for the game checkers

int basic_evaluation(board &b){
    return (b.bcount()*- b.wcount()) * evalhyp[0]["pawn_value"] + (b.bkcount()* - b.wkcount())*evalhyp[0]["king_value"]; 
}

int evaluate(board &b){
    switch(generalhyp["evalution_alg"]){
        case 0:
            return basic_evaluation(b);
        default:
            return basic_evaluation(b);
    }
}

std::pair<int, move> minimax(board &b, int leftdepth, bool maximazing = true, int alpha = INT32_MIN, int beta = INT32_MAX){
    move bestmove;
    if(leftdepth==0)
        return {evaluate(b), bestmove};
    int bestscore = evaluate(b);
    for(move nextmove: b.moves())
    {
        b.play(nextmove);

        std::pair<int, move> moveinfo = minimax(b, leftdepth-1, !maximazing, alpha, beta);
        if((moveinfo.first<bestscore)^maximazing){
            bestscore = moveinfo.first;
            bestmove = nextmove;
        }

        b.undo(nextmove); 

        if(maximazing)
            alpha = std::max(alpha, bestscore);
        else
            beta = std::min(beta, bestscore);
        
        if(beta<=alpha)
            break;        
    }

    return {bestscore, bestmove};
}

move findmove(board &b, int maxdepth = 6){
    move bestmove;
    switch(generalhyp["search_alg"]){
        case 0:
            bestmove = minimax(b, maxdepth).second;
            break;
        default:
            break;
    }
    return bestmove;
}
