#include "implementations/PlayerOne.h"

void PlayerOne::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // player updates
    auto *tankPtr = dynamic_cast<ConcreteTankAlgorithm *>(&tank);
    int tankId = tankPtr->getTankId();

    analyzeBoard(tankId, satellite_view);
    handleTankTurnReordering(tankId);
    playerState.turnId = tankTurns[tankId];

    // Create battle info with current player state
    ConcreteBattleInfo info(playerState);

    // tanks updates
    tank.updateBattleInfo(info);
    ConcreteBattleInfo::TankUpdates tankUpdates = info.getTankUpdates();

    remainingShells[tankId] = tankUpdates.remainingShells;
    roundCounter = tankUpdates.roundCounter;
    playerState.myTanksInfo[tankId].second = tankUpdates.tankDir;
}

