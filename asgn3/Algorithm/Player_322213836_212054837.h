//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_PLAYER_322213836_212054837_H
#define ASGN3_PLAYER_322213836_212054837_H

#include "Common/Player.h"
#include "Common/TankAlgorithm.h"
#include "Common/SatelliteView.h"
#include <vector>

namespace Algorithm_322213836_212054837 {

using std::unique_ptr;
using std::vector;

class Player_322213836_212054837: public Player {
public:
    Player_322213836_212054837(int player_id, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : playerId(player_id), startX(x), startY(y), maxSteps(max_steps), numShells(num_shells) {}

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override {
        // Create battle info with current game state
        BattleInfo info;
        // Update tank's battle info
        tank.updateBattleInfo(info);
    }

private:
    int playerId;
    size_t startX;
    size_t startY;
    size_t maxSteps;
    size_t numShells;
};

}

#endif //ASGN3_PLAYER_322213836_212054837_H
