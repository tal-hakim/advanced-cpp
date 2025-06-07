#ifndef PLAYER_ONE_H
#define PLAYER_ONE_H

#include "ConcretePlayer.h"
#include "ConcreteBattleInfo.h"
#include "ConcreteTankAlgorithm.h"
#include <iostream>
#include <limits>

class PlayerOne : public ConcretePlayer {
public:
    PlayerOne(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
            : ConcretePlayer(playerIndex, x, y, maxSteps, numShells) {}


    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
};

#endif // PLAYER_ONE_H

