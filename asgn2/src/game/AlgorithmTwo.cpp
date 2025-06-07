#include "game/AlgorithmTwo.h"
using namespace MapUtils;
void AlgorithmTwo::updateBattleInfo(BattleInfo& info) {
    // First call base class implementation to update all the standard fields
    ConcreteTankAlgorithm::updateBattleInfo(info);
    
    // Now update our BFS path from the battle info
    auto* concreteInfo = dynamic_cast<ConcreteBattleInfo*>(&info);
    if (concreteInfo) {
        auto updates = concreteInfo->getPlayerUpdates();
        bfsPath = updates.bfsToEnemy;  // Update our bfsPath from the battle info
    }
}

ActionRequest AlgorithmTwo::chaseEnemy(Position myPos, Direction myDir) {
    // We should have exactly one enemy in enemyTanksInfo from battle info

    Position enemyPos = playerState.enemyTanksInfo[0].first;
    // Don't recalculate path - use the one from battle info
    // If path is empty, we need new battle info
    if (bfsPath.empty()) {
        return ActionRequest::GetBattleInfo;
    }

    if (canShootEnemy(myDir, myPos, enemyPos)) {
        return shoot();
    }

    if (bfsPath.size() < 7) {
        for (Direction dir : getAllDirections()) {
            if (canShootEnemy(dir, myPos, enemyPos)) {
                if (dir != myDir) {
                    return rotateToward(myDir, dir);
                }
                return shoot();
            }
        }
    }

    if (bfsPath.size() < 2) {
        Direction desired = DirectionUtils::directionFromTo(myPos, enemyPos);
        if (myDir != desired) {
            return rotateToward(myDir, desired);
        }
        if (canShoot()){
            return shoot();
        }
        return ActionRequest::GetBattleInfo;
    }

    Position next = bfsPath[1];
    Direction desired = DirectionUtils::directionFromTo(myPos, next);

    if (myDir != desired) {
        return rotateToward(myDir, desired);
    }

    if (!isWalkable(next, playerState.latestMap)) {
        // If path is blocked, we need new battle info to get a new path
        return ActionRequest::GetBattleInfo;
    }

    bfsPath.erase(bfsPath.begin());
    if(canMoveForward(myPos, myDir)) {
        return ActionRequest::MoveForward;
    }
    
    // If we can't move forward, try to find a safe direction to rotate to
    for (Direction dir : getSafeRotations(myPos)) {
        if (myDir == dir) {
            continue;  // Skip if we're already facing this direction
        }
        return rotateToward(myDir, dir);
    }
    
    // If we can't find any safe moves, get new battle info
    return ActionRequest::GetBattleInfo;
}

ActionRequest AlgorithmTwo::getAction() {
    tankState.roundCounter++;

    // Update shell positions at the start of each action
    updateShellPositions();

    if (shouldGetBattleInfo()) {
        return ActionRequest::GetBattleInfo;
    }

    Position myPos = playerState.myTanksInfo[tankId].first;
    Direction myDir = tankState.tankDir;

    // If we have ammo, use chaseEnemy behavior
    if (tankState.remainingShells > 0 && !bfsPath.empty()) {
        return chaseEnemy(myPos, myDir);
    }

    // If we're out of ammo, use evadeThreat behavior
    ThreatInfo threat = detectThreat(myPos);
    // When out of ammo, use getNearestEnemy to find a threat
    auto enemyPos = getNearestEnemy(myPos);
    if (enemyPos.has_value()) {
        threat.exists = true;
        threat.pos = enemyPos.value();
        threat.dir = DirectionUtils::directionFromTo(enemyPos.value(), myPos);
        threat.steps = std::abs(enemyPos.value().x - myPos.x) + std::abs(enemyPos.value().y - myPos.y);
    }

    return evadeThreat(threat, myPos, myDir);
}