#ifndef SIMPLE_PLAYER_H
#define SIMPLE_PLAYER_H
#include "PlayerImp.h"
#include "SatelliteViewImp.h"
#include <iostream>
#include <vector>
#include <utility> // for std::pair

class SimplePlayer : public PlayerImp {
    public:
        SimplePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
            : PlayerImp(player_index, x, y, max_steps, num_shells)           
        {}
        virtual ~SimplePlayer() = default;
        virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};

#endif