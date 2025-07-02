//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_TANKALGORITHM_322213836_212054837_H
#define ASGN3_TANKALGORITHM_322213836_212054837_H

#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/ActionRequest.h"
#include "../UserCommon/utils/Direction.h"
#include "../UserCommon/utils/Position.h"
#include "../UserCommon/utils/definitions.h"
#include "../UserCommon/utils/MapUtils.h"
#include "../UserCommon/utils/DirectionUtils.h"
#include "ConcreteBattleInfo.h"
#include <optional>
#include <queue>
#include <limits>

using namespace UserCommon_322213836_212054837;

namespace FakeAlgorithm {

using std::unique_ptr;

class FakeTankAlgorithm : public TankAlgorithm {
    private:
        int playerId;
        size_t numTanks;  // Track total number of tanks in the game
        size_t mapWidth;  // Map width, updated with battle info
        size_t mapHeight; // Map height, updated with battle info
        Position lastEnemyPos;  // Store last known enemy position for path recalculation
        int lastShotRound;  // Track the last round we shot
    protected:
        int tankId;
        std::vector<Position> bfsPath;  // Store the current BFS path



    public:


        FakeTankAlgorithm(int player_index, int tank_index)
                : playerId(player_index), numTanks(0),
                  mapWidth(static_cast<size_t>(UNINITIALIZED)), mapHeight(static_cast<size_t>(UNINITIALIZED)), lastEnemyPos(Position{0,0}),
                  lastShotRound(-4), tankId(tank_index) {

        }
        virtual ~FakeTankAlgorithm() = default;

        void updateBattleInfo(BattleInfo& info) override {};

        ActionRequest getAction() override;

    };

}

#endif //ASGN3_TANKALGORITHM_322213836_212054837_H
