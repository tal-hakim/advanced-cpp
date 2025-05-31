#include "game/ConcreteBattleInfo.h"
#include "common/SatelliteView.h"

ConcreteBattleInfo::ConcreteBattleInfo(PlayerUpdates updates) 
    : playerUpdates(updates) {
    // Initialize tank updates with default values
    tankUpdates.tankDir = Direction::R;
    tankUpdates.remainingShells = 0;
    tankUpdates.roundCounter = 0;
} 