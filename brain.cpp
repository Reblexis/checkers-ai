#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"
#include "cache.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

int basic_evaluation(board &b){
    if(b.whitewin)
        return INT32_MIN;
    if(b.blackwin)
        return INT32_MAX;
    if(b.draw)
        return 0;

    return (b.bpcount() - b.wpcount()) * evalhyp[0]["pawn_value"] + (b.bkcount() - b.wkcount())*evalhyp[0]["king_value"]; 
}


int advanced_evaluation(board &b)
{
    if(b.whitewin)
        return INT32_MIN;
    if(b.blackwin)
        return INT32_MAX;
    if(b.draw)
        return 0;

    int score = 0;
    int pawnposscore = 0;
    int kingposscore = 0;

    // Add to pawnposscore score for pawns at the positions specified in the pawn table
    // White has the table flipped
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(b.b & (1 << i))
            pawnposscore += pawntable[i]; 
        else if(b.w & (1 << i))
            pawnposscore -= pawntable[NUM_SQUARES-i];
    }
    
    // Add score for kings at the positions specified in the king table
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(b.bk & (1 << i))
            kingposscore += kingtable[i];
        else if(b.wk & (1 << i))
            kingposscore -= kingtable[NUM_SQUARES-i];
    }       

    // Add score for the difference 

    int pawndiff = b.bpcount() - b.wpcount();
    int kingdiff = b.bkcount() - b.wkcount();
    pawndiff *= evalhyp[1]["pawn_value"];
    kingdiff *= evalhyp[1]["king_value"];
    kingposscore *= evalhyp[1]["king_value"];
    pawnposscore *= evalhyp[1]["pawn_value"];

    // Add preference for having bigger difference with less pieces

    score = pawndiff + kingdiff + kingposscore + pawnposscore;
    
    return score;
}

int evaluate(board &b){
    switch(generalhyp["evalution_alg"]){
        case 0:
            return basic_evaluation(b);
        case 1:
            return advanced_evaluation(b);
        default:
            return basic_evaluation(b);
    }
}

std::pair<int, move> minimax(board &b, cache &c, int leftdepth, bool maximazing = true, int alpha = INT32_MIN, int beta = INT32_MAX, bool usecache = true){
    move bestmove;

    if(usecache&&leftdepth!=searchhyp[0]["max_depth"]){
        cacheval cacheinfo = c.get(b, leftdepth);
        if(leftdepth<=cacheinfo.depth)
            return {cacheinfo.score, bestmove};
    }
    
    if(leftdepth==0)
        return {evaluate(b), bestmove};
    int bestscore = evaluate(b);
    for(move nextmove: b.moves())
    {
        b.play(nextmove);

        std::pair<int, move> moveinfo = minimax(b, c, leftdepth-1, !maximazing, alpha, beta);
        if((moveinfo.first<bestscore)^maximazing){
            bestscore = moveinfo.first;
            bestmove = nextmove;
        }

        b.undo(); 

        if(maximazing)
            alpha = std::max(alpha, bestscore);
        else
            beta = std::min(beta, bestscore);
        
        if(beta<=alpha)
            break;
    }

    if(usecache)
        c.set(b, leftdepth, bestscore);

    return {bestscore, bestmove};
}

move findmove(board &b, cache &c){
    move bestmove;
    switch(generalhyp["search_alg"]){
        case 0:
            bestmove = minimax(b, c, searchhyp[0]["max_depth"]).second;
            break;
        default:
            break;
    }
    return bestmove;
}
