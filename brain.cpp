#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"
#include "cache.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

int get_board_state(board &b){
    // 0 in-game 1 white win 2 black win 3 draw
    movelist moves = b.moves();
    if(moves.size()==0)
        return 2-b.next;
    if(b.w==0)
        return 2;
    if(b.b==0)
        return 1;
    return 0;
}

int basic_evaluation(board &b, int leftdepth){
    int gamestate = get_board_state(b);

    return (b.bpcount() - b.wpcount()) * allhyperparams[EH_B_PAWN_VALUE] + (b.bkcount() - b.wkcount())*allhyperparams[EH_B_KING_VALUE]; 
}


int advanced_evaluation(board &b, int leftdepth)
{
    int gamestate = get_board_state(b);
    if(gamestate==1)
        return INT32_MIN;
    if(gamestate==2)
        return INT32_MAX;
    if(gamestate==3)
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
    pawndiff *= allhyperparams[EH_B_PAWN_VALUE];
    kingdiff *= allhyperparams[EH_B_KING_VALUE];
    kingposscore *= allhyperparams[EH_B_KING_VALUE];
    pawnposscore *= allhyperparams[EH_B_PAWN_VALUE];
    
    // Add preference for having bigger difference with less pieces

    score = pawndiff + kingdiff + kingposscore + pawnposscore;
    int leftdepthmultiplier = sqrt(leftdepth)/allhyperparams[EH_A_DEPTH_DIVISOR];
    score *= leftdepthmultiplier;

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

std::pair<int, move> minimax(board &b, cache &c, int leftdepth=0, bool maximazing = true, int alpha = INT32_MIN, int beta = INT32_MAX, bool usecache = true){
    move bestmove;

    if(usecache&&leftdepth!=allhyperparams[SH_MAX_DEPTH]){
        cacheval cacheinfo = c.get(b, leftdepth);
        if(leftdepth<=cacheinfo.depth)
            return {cacheinfo.score, bestmove};
    }
    
    if(leftdepth==0)
        return {evaluate(b, leftdepth), bestmove};

    int bestscore = maximazing?INT32_MIN:INT32_MAX;
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
    switch(allhyperparams[GH_SEARCH_ALG]){
        case 0:
            bestmove = minimax(b, c).second;
            break;
        default:
            break;
    }
    return bestmove;
}
