#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"
#include "cache.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

cache<> c;

int basic_evaluation(board &b, int leftdepth){
    if(b.moves().size()==0)
        return b.nextblack?-INT32_MAX:INT32_MAX;
    return (b.bpcount() - b.wpcount()) * allhyperparams[EH_B_PAWN_VALUE] + (b.bkcount() - b.wkcount())*allhyperparams[EH_B_KING_VALUE]; 
}


int advanced_evaluation(board &b, int leftdepth)
{
    if (b.moves().size() == 0)
        return b.nextblack ? -INT32_MAX : INT32_MAX;

    int score = 0;
    int pawnposscore = 0;
    int kingposscore = 0;

    // Add to pawnposscore score for pawns at the positions specified in the pawn table
    // Black has the table flipped
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(b.b & (1 << i))
            pawnposscore += pawntable[NUM_SQUARES-i]; 
        else if(b.w & (1 << i))
            pawnposscore -= pawntable[i];
    }
    
    // Add score for kings at the positions specified in the king table
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(b.bk & (1 << i))
            kingposscore += kingtable[NUM_SQUARES-i];
        else if(b.wk & (1 << i))
            kingposscore -= kingtable[i];
    }       

    // Add score for the difference 

    int pawndiff = b.bpcount() - b.wpcount();
    int kingdiff = b.bkcount() - b.wkcount();
    pawndiff *= allhyperparams[EH_B_PAWN_VALUE]*allhyperparams[EH_A_DIFF_MULTIPLIER];
    kingdiff *= allhyperparams[EH_B_KING_VALUE]*allhyperparams[EH_A_DIFF_MULTIPLIER];
    kingposscore *= allhyperparams[EH_B_KING_VALUE];
    pawnposscore *= allhyperparams[EH_B_PAWN_VALUE];
    
    // Add preference for having bigger difference with less pieces

    score = pawndiff + kingdiff + kingposscore + pawnposscore;
    //int leftdepthmultiplier = sqrt(leftdepth)/allhyperparams[EH_A_DEPTH_DIVISOR];
    // score *= leftdepthmultiplier;

    return score;
}

int evaluate(board &b, int leftdepth){
    switch(allhyperparams[GH_EVALUATION_ALG]){
        case 0:
            return basic_evaluation(b, leftdepth);
        case 1:
            return advanced_evaluation(b, leftdepth);
        default:
            return basic_evaluation(b, leftdepth);
    }
}

std::pair<int, move> minimax(board &b, int leftdepth, int alpha = INT32_MIN, int beta = INT32_MAX, bool usecache = true){
    move bestmove=0;
    bool maximazing = b.nextblack;
    int bestscore = maximazing?INT32_MIN:INT32_MAX;

    if(allhyperparams[SH_USE_CACHE]&&leftdepth!=allhyperparams[SH_MAX_DEPTH]){
        const cache_entry &cacheinfo = c.get(b.hash);
        if(leftdepth+1 <= cacheinfo.depth)
            return {cacheinfo.score, 0};
    }
    
    if(leftdepth==0)
        return {evaluate(b, leftdepth), bestmove};

    for(move nextmove: b.moves())
    {
        b.play(nextmove);

        std::pair<int, move> moveinfo = minimax(b, leftdepth-1, alpha, beta);
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

    if(allhyperparams[SH_USE_CACHE])
        c.set(b.hash, leftdepth+1, bestscore);

    return {bestscore, bestmove};
}

std::pair<int, move> findmove(board &b) {
    std::pair<int, move> bestmove;
    switch(allhyperparams[GH_SEARCH_ALG]){
        case 0:
            bestmove = minimax(b, allhyperparams[SH_MAX_DEPTH]);
            break;
        default:
            break;
    }
    return bestmove;
}
