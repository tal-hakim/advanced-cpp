//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_TANKALGORITHM_322213836_212054837_H
#define ASGN3_TANKALGORITHM_322213836_212054837_H

#include "Common/TankAlgorithm.h"
#include "Common/BattleInfo.h"
#include "Common/ActionRequest.h"
#include <random>

namespace Algorithm_322213836_212054837 {

using std::unique_ptr;

class TankAlgorithm_322213836_212054837 : public TankAlgorithm {
public:
    TankAlgorithm_322213836_212054837(int player_id, int tank_id) 
        : playerId(player_id), tankId(tank_id), rng(std::random_device{}()) {}

    ActionRequest getAction() override {
        // Simple strategy: randomly choose between moving, rotating, and shooting
        std::uniform_int_distribution<int> dist(0, 7);
        int action = dist(rng);

        switch (action) {
            case 0: return ActionRequest::MoveForward;
            case 1: return ActionRequest::MoveBackward;
            case 2: return ActionRequest::RotateLeft90;
            case 3: return ActionRequest::RotateRight90;
            case 4: return ActionRequest::RotateLeft45;
            case 5: return ActionRequest::RotateRight45;
            case 6: return ActionRequest::Shoot;
            default: return ActionRequest::GetBattleInfo;
        }
    }

    void updateBattleInfo(BattleInfo& info) override {
        // Store the battle info for future decisions
        lastBattleInfo = info;
    }

private:
    int playerId;
    int tankId;
    BattleInfo lastBattleInfo;
    std::mt19937 rng;
};

}

#endif //ASGN3_TANKALGORITHM_322213836_212054837_H
