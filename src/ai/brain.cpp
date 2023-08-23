#include "../communication/includes/interface.hpp"
#include <bits/stdc++.h>
#include "includes/hyperparameters.hpp"
#include "includes/cache.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

extern cache<> c;

int basic_evaluation(Board &b){

}

int advanced_evaluation(Board &b)
{

}
int evaluate(Board &b){
    switch(currenthyperparams[GH_EVALUATION_ALG]){
        case 0:
            return basic_evaluation(b);
        case 1:
            return advanced_evaluation(b);
        default:
            return basic_evaluation(b);
    }
}

long long ops = 0;
bool finished = true;

template<bool toplevel=true>
std::pair<int, move> minimax(Board &b, int leftdepth, int alpha = INT32_MIN, int beta = INT32_MAX){
    ops++;

    bool maximizing = b.nextblack;
    int bestscore = maximizing?INT32_MIN:INT32_MAX;
    
    move bestmove=0;
 
	if constexpr(!toplevel) { // check cache
 	   if(currenthyperparams[SH_USE_CACHE]){
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
		int score = evaluate(b);
    	if(currenthyperparams[SH_USE_CACHE])
    	    c.set(b.hash, 1, score, 0);
        return {score, 0};
	}

    moveList possiblemoves = b.moves();
	if (possiblemoves.size() == 0) {
		return {maximizing ? INT32_MIN : INT32_MAX, 0};
	}
    if (!bestmove)
		bestmove = *possiblemoves.begin();
    for(move nextmove: possiblemoves)
    {
        if(ops>=currenthyperparams[SH_OPERATION_LIMIT]&&currenthyperparams[GH_SEARCH_ALG]==0)
        {
            finished = false;
            return {0, 0};
        }
        b.play(nextmove);

        std::pair<int, move> moveinfo = minimax<false>(b, leftdepth-1, alpha, beta);
        if(maximizing ? moveinfo.first > bestscore : moveinfo.first < bestscore){
            bestscore = moveinfo.first;
            bestmove = nextmove;
        }

        b.undo(); 

        if(maximizing)
            alpha = std::max(alpha, bestscore);
        else
            beta = std::min(beta, bestscore);
        
        if(beta<=alpha)
            break;
    }

	// update cache and return
    if(currenthyperparams[SH_USE_CACHE])
       	c.set(b.hash, leftdepth+1, bestscore, bestmove);
	return {bestscore, bestmove};
}

std::pair<int, move> iterative_minimax(Board &b, int maxdepth){
    std::pair<int, move> bestmove;
    bestmove.first = b.nextblack ? INT32_MIN : INT32_MAX;
    ops = 0;
    finished = true;

    for(int i = 1; i <= currenthyperparams[SH_MAX_DEPTH] && ops<currenthyperparams[SH_OPERATION_LIMIT]; i++){
		//std::cerr << "depth " << i << std::flush;
		auto starttime = std::chrono::high_resolution_clock::now();

        std::pair<int, move> candidate = minimax(b, i);
        if(!finished)
            break;

        bestmove = candidate;

        if((bestmove.first==INT32_MAX&&b.nextblack) || (bestmove.first==INT32_MIN&&!b.nextblack))
        {
            //std::cout<<"Found forced win / loss at depth "<<i<<"\n";
            break;
        }
		auto endtime = std::chrono::high_resolution_clock::now();
		//std::cerr << " [" << std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count() <<
		//	"ms]\n"; // {" << bestmove.first << "} " << visualizeMove(bestmove.second);
    }

    return bestmove;
}

std::pair<int, move> findmove(Board &b) {
    std::pair<int, move> bestmove;
    switch(currenthyperparams[GH_SEARCH_ALG]){
        case 0:
            bestmove = iterative_minimax(b, currenthyperparams[SH_MAX_DEPTH]);
            break;
		case 1:
			{
				moveList ml = b.moves();
				if (ml.size() == 0)
					return {0, 0};
				return {0, ml.begin()[rand64(b.hash) % ml.size()]};
			}
        default:
		case 2:
			bestmove = minimax(b, currenthyperparams[SH_MAX_DEPTH]);
            break;
    }
    return bestmove;
}
