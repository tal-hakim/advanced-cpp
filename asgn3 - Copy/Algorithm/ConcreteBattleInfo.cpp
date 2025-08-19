#include "ConcreteBattleInfo.h"
#include "../common/SatelliteView.h"
namespace Algorithm_322213836_212054837 {
    ConcreteBattleInfo::ConcreteBattleInfo(PlayerUpdates updates)
            : playerUpdates(updates) {
        // Initialize tank updates with default values
        tankUpdates.tankDir = Direction::R;
        tankUpdates.remainingShells = 0;
        tankUpdates.roundCounter = 0;
    }

    void ConcreteBattleInfo::setEnemy(std::pair<Position, Direction> enemy) {
        playerUpdates.enemyTanksInfo.clear();
        playerUpdates.enemyTanksInfo.emplace_back(enemy);  // cleaner and avoids copy
    }
}