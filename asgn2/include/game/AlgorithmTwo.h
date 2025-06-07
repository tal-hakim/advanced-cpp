//
// Created by talta on 04/06/2025.
//

#ifndef ASGN2_ALGORITHMTWO_H
#define ASGN2_ALGORITHMTWO_H
#include "game/ConcreteTankAlgorithm.h"
class AlgorithmTwo : public ConcreteTankAlgorithm{
public:
    AlgorithmTwo(int player_index, int tank_index) : ConcreteTankAlgorithm(player_index, tank_index){};
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    ActionRequest chaseEnemy(Position myPos, Direction myDir) override;
};

#endif //ASGN2_ALGORITHMTWO_H
