//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_BATTLEINFO_H
#define ASGN2_BATTLEINFO_H

class BattleInfo {
private:
    int gameStep;

public:
    BattleInfo() : gameStep(0) {}
    virtual ~BattleInfo() {}
    
    void setGameStep(int step) { gameStep = step; }
    int getGameStep() const { return gameStep; }
};

#endif //ASGN2_BATTLEINFO_H
