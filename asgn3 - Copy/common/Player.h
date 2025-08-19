//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_PLAYER_H
#define ASGN3_PLAYER_H

#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <memory>
#include <functional>

class Player {
public:
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

using PlayerFactory =
        std::function<std::unique_ptr<Player>
        (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)>;
#endif //ASGN3_PLAYER_H
