//
// Created by talta on 21/06/2025.
//

#include "../common/PlayerRegistration.h"
#include "FakePlayer.h"
#include "ConcreteBattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "FakeTankAlgorithm.h"
#include "../UserCommon/utils/MapUtils.h"
#include "../UserCommon/utils/definitions.h"
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

using namespace FakeAlgorithm;
REGISTER_PLAYER(FakePlayer);

namespace FakeAlgorithm {
    using namespace MapUtils;  // Add MapUtils namespace at file scope

    FakePlayer::FakePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
            : playerId(playerIndex), boardWidth(x), boardHeight(y), maxSteps(maxSteps), numShells(numShells), aliveTanks(UNINITIALIZED), reorderTurns(false){
        // Initialize context with empty state
    }

    void FakePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    }

}