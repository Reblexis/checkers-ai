#include "interface.hpp"
#include <bits/stdc++.h>
#include "hyperparams.hpp"
#include "cache.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

extern cache<> c;

int basic_evaluation(board &b, int leftdepth){
    if(b.moves().size()==0)
        return b.nextblack ? INT32_MIN : INT32_MAX;
    return (b.bpcount() - b.wpcount()) * allhyperparams[EH_B_PAWN_VALUE] + (b.bkcount() - b.wkcount())*allhyperparams[EH_B_KING_VALUE]; 
}

int advanced_evaluation(board &b, int leftdepth)
{
    if (b.moves().size() == 0)
        return b.nextblack ? INT32_MIN : INT32_MAX;

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
    pawndiff *= allhyperparams[EH_A_PAWN_VALUE]*allhyperparams[EH_A_DIFF_MULTIPLIER];
    kingdiff *= allhyperparams[EH_A_KING_VALUE]*allhyperparams[EH_A_DIFF_MULTIPLIER];
    kingposscore *= allhyperparams[EH_A_KING_VALUE];
    pawnposscore *= allhyperparams[EH_A_PAWN_VALUE];

    score = pawndiff + kingdiff + kingposscore + pawnposscore;
    //float leftdepthmultiplier = (float)((leftdepth-curdepthlim)+20)/(float)+5;
    //float fscore = (float)score*leftdepthmultiplier;

    //score = (int)fscore;

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

long long ops = 0;

template<bool toplevel=true>
std::pair<int, move> minimax(board &b, int leftdepth, int alpha = INT32_MIN, int beta = INT32_MAX){
    ops++;

    bool maximazing = b.nextblack;
    int bestscore = maximazing?INT32_MIN:INT32_MAX;
    
    move bestmove=0;
 
	if constexpr(!toplevel) { // check cache
 	   if(allhyperparams[SH_USE_CACHE]){
 	       const cache_entry &cacheinfo = c.get(b.hash);
 	       if(leftdepth+1 <= cacheinfo.depth)
 	           return {cacheinfo.score, cacheinfo.best};
            if(cacheinfo.depth!=0){
                bestmove = cacheinfo.best;
                bestscore = cacheinfo.score;
            }
 	   }
	}
    
    if(leftdepth==0) { // eval at leaf nodes
		int score = evaluate(b, leftdepth);
    	if(allhyperparams[SH_USE_CACHE])
    	    c.set(b.hash, 1, score, 0);
        return {score, 0};
	}

    movelist possiblemoves = b.moves();
    for(move nextmove: possiblemoves)
    {
        b.play(nextmove);

        std::pair<int, move> moveinfo = minimax<false>(b, leftdepth-1, alpha, beta);
        if(((moveinfo.first>=bestscore)&&maximazing)||((moveinfo.first<=bestscore)&&!maximazing)){
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

	// update cache and return
    if(allhyperparams[SH_USE_CACHE])
       	c.set(b.hash, leftdepth+1, bestscore, bestmove);
	return {bestscore, bestmove};
}

std::pair<int, move> iterative_minimax(board &b, int maxdepth){
    std::pair<int, move> bestmove;
    ops = 0;
    for(int i = 3; i <= allhyperparams[SH_MAX_DEPTH] && ops<allhyperparams[SH_OPERATION_LIMIT]; i++){
		//std::cerr << "depth " << i << std::flush;
		auto starttime = std::chrono::high_resolution_clock::now();
        bestmove = minimax(b, i);
        if((bestmove.first==INT32_MAX&&b.nextblack) || (bestmove.first==INT32_MIN&&!b.nextblack))
        {
            //std::cout<<"Found forced win / loss at depth "<<i<<"\n";
            break;
        }
		auto endtime = std::chrono::high_resolution_clock::now();
		//std::cerr << " [" << std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count() <<
		//	"ms]\n"; // {" << bestmove.first << "} " << move_vis(bestmove.second);
    }
    return bestmove;
}

std::pair<int, move> findmove(board &b) {
    std::pair<int, move> bestmove;
    switch(allhyperparams[GH_SEARCH_ALG]){
        case 0:
            bestmove = iterative_minimax(b, allhyperparams[SH_MAX_DEPTH]);
            break;
		case 1:
			{
				movelist ml = b.moves();
				if (ml.size() == 0)
					return {0, 0};
				return {0, ml.begin()[rand64(b.hash) % ml.size()]};
			}
        default:
            break;
    }
    return bestmove;
}
