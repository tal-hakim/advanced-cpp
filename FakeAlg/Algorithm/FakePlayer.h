//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_PLAYER_322213836_212054837_H
#define ASGN3_PLAYER_322213836_212054837_H

#include "../common/Player.h"
#include "../common/TankAlgorithm.h"
#include "../common/SatelliteView.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "ConcreteBattleInfo.h"
#include "../UserCommon/utils/Position.h"
#include <vector>
using namespace UserCommon_322213836_212054837;
namespace FakeAlgorithm {

using std::unique_ptr;
using std::vector;

class FakePlayer: public Player {
    private:
        // Base class parameters

        size_t maxSteps;
        size_t numShells;

        // State variables
        int aliveTanks;
        bool reorderTurns;


        // Helper functions for board analysis
    protected:
        int playerId;
        size_t boardWidth;
        size_t boardHeight;


    public:
        FakePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
        ~FakePlayer() = default;
        void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
    };

}

#endif //ASGN3_PLAYER_322213836_212054837_H
