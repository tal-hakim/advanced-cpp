//
// Created by talta on 24/06/2025.
//

#include "GameContainer.h"
std::map<std::string, int> GameContainer::getGameScore(){
    std::map<std::string, int> scores;
    scores[alg1Name] = LOSING_SCORE;
    scores[alg2Name] = LOSING_SCORE;
    if (gameResult.winner == TIE){
        scores[alg1Name] = TIE_SCORE;
        scores[alg2Name] = TIE_SCORE;
        return scores;
    }
    if (gameResult.winner == PLAYER_1_ID){
        scores[alg1Name] = WINNING_SCORE;
    } else{
        scores[alg2Name] = WINNING_SCORE;
    }
    return scores;
}