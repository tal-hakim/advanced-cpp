//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_PLAYER_H
#define ASGN2_PLAYER_H

#include "TankAlgorithm.h"
#include "SatelliteView.h"

class Player {
public:
    Player( int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells ) {}
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

#endif //ASGN2_PLAYER_H
