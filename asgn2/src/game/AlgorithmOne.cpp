#include "game/AlgorithmOne.h"

ActionRequest AlgorithmOne::getAction() {
    tankState.roundCounter++;

    // Update shell positions at the start of each action
    updateShellPositions();

    if (shouldGetBattleInfo()) {
        return ActionRequest::GetBattleInfo;
    }

    Position myPos = playerState.myTanksInfo[tankId].first;
    Direction myDir = tankState.tankDir;

    ThreatInfo threat = detectThreat(myPos);
    if (tankState.remainingShells <= 0) {
        // When out of ammo, use getNearestEnemy to find a threat
        auto enemyPos = getNearestEnemy(myPos);
        if (enemyPos.has_value()) {
            threat.exists = true;
            threat.pos = enemyPos.value();
            threat.dir = DirectionUtils::directionFromTo(enemyPos.value(), myPos);
            threat.steps = std::abs(enemyPos.value().x - myPos.x) + std::abs(enemyPos.value().y - myPos.y);
        }
    }

    if (threat.exists || tankState.remainingShells <= 0) {
        return evadeThreat(threat, myPos, myDir);
    }

    return chaseEnemy(myPos, myDir);
}


