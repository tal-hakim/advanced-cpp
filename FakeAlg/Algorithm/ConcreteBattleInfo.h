#ifndef CONCRETE_BATTLE_INFO_H
#define CONCRETE_BATTLE_INFO_H

#include "../common/BattleInfo.h"
#include "../UserCommon/utils/Direction.h"
#include "../UserCommon/utils/Position.h"
#include "../common/SatelliteView.h"
#include <utility>
#include <vector>
#include <map>
#include <utility>
#include <unordered_map>

namespace FakeAlgorithm {
    using namespace UserCommon_322213836_212054837;

    class ConcreteBattleInfo : public BattleInfo {
    public:
        struct PlayerUpdates {
            // Changed to match BattleInfo format: playerId -> vector of (position, direction) pairs
            std::vector<std::pair<Position, Direction>> myTanksInfo;
            std::vector<std::pair<Position, Direction>> enemyTanksInfo;
            std::vector<std::vector<char>> latestMap;
            std::unordered_map<Position, Direction> shellInfo;  // Maps shell positions to their directions
            size_t turnId;
            int initShells;
            std::vector<Position> bfsToEnemy;  // Store the BFS path to the enemy
        };
        struct TankUpdates {
            Direction tankDir;
            int remainingShells;
            size_t roundCounter = 0;
        };

        // Constructor takes initial state from player
        ConcreteBattleInfo(PlayerUpdates updates);

        ~ConcreteBattleInfo() override = default;

        ConcreteBattleInfo(const ConcreteBattleInfo &) = delete;

        ConcreteBattleInfo &operator=(const ConcreteBattleInfo &) = delete;

        // Getters for player to read the updated state
        PlayerUpdates getPlayerUpdates() const { return playerUpdates; }

        TankUpdates getTankUpdates() const { return tankUpdates; }

        void setPlayerUpdates(const PlayerUpdates &updates) {
            playerUpdates = updates;
        }

        void setTankUpdates(const TankUpdates &updates) {
            tankUpdates = updates;
        }

        void setEnemy(std::pair<Position, Direction> enemy);

    private:
        PlayerUpdates playerUpdates;
        TankUpdates tankUpdates;


    };
}
#endif // CONCRETE_BATTLE_INFO_H 