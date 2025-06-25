//
// Created by talta on 24/06/2025.
//

#include "GameContainer.h"
std::map<std::string, int> GameContainer::getGameScore() const{
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

bool GameContainer::isSameResult(const GameContainer& other) const {
    const GameResult& a = this->gameResult;
    const GameResult& b = other.gameResult;

    if (a.winner != b.winner) return false;
    if (a.reason != b.reason) return false;
    if (a.rounds != b.rounds) return false;
    if (a.remaining_tanks != b.remaining_tanks) return false;

    // Simplified board state check:
    if (bool(a.gameState) != bool(b.gameState)) return false;
    if (a.gameState && b.gameState) {
        for (size_t y = 0; y < initInfo.height; ++y) {
            for (size_t x = 0; x < initInfo.width; ++x) {
                if (a.gameState->getObjectAt(x, y) != b.gameState->getObjectAt(x, y))
                    return false;
            }
        }
    }
    return true;
}