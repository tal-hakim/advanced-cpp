//
// Created by talta on 21/06/2025.
//

#include "../Common/TankAlgorithmRegistration.h"
#include "TankAlgorithm_322213836_212054837.h"


namespace Algorithm_322213836_212054837 {
    using namespace MapUtils;
    REGISTER_TANK_ALGORITHM(TankAlgorithm_322213836_212054837);
// Helper function to determine rotation action
    ActionRequest TankAlgorithm_322213836_212054837::rotateToward(Direction current, Direction target) {
        if (current == target) {
            // TODO: if cant move forward - handle it
            return ActionRequest::MoveForward;
        }

        int currentAngle = static_cast<int>(current);
        int targetAngle = static_cast<int>(target);
        int diff = (targetAngle - currentAngle + NUM_DIRECTIONS) % NUM_DIRECTIONS;

        if (diff == 1) {
            tankState.tankDir = static_cast<Direction>((currentAngle + 1) % NUM_DIRECTIONS);
            return ActionRequest::RotateRight45;
        }
        if (diff == 2 || diff == 3 || diff == 4) {
            tankState.tankDir = static_cast<Direction>((currentAngle + 2) % NUM_DIRECTIONS);
            return ActionRequest::RotateRight90;
        }
        if (diff == 7) {
            tankState.tankDir = static_cast<Direction>((currentAngle + 7) % NUM_DIRECTIONS);
            return ActionRequest::RotateLeft45;
        }
        if (diff == 6 || diff == 5) {
            tankState.tankDir = static_cast<Direction>((currentAngle + 6) % NUM_DIRECTIONS);
            return ActionRequest::RotateLeft90;
        }

        return ActionRequest::DoNothing;
    }

//
//    bool isTank(const std::vector<std::vector<char>> &map, const Position &pos) {
//        char c = map[pos.x][pos.y];
//        return c >= '1' && c <= '9';
//    }


    ActionRequest TankAlgorithm_322213836_212054837::getAction() {
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

// ====== Public Method ======

// ====== Top-Level Helpers ======
    bool TankAlgorithm_322213836_212054837::shouldGetBattleInfo() const {
        return ((tankState.roundCounter == 0 ||
                 (tankState.roundCounter % static_cast<int>(numTanks) == playerState.turnId))
                && numTanks != 1) || (numTanks == 1 && tankState.roundCounter % 3 == 0);
    }

    TankAlgorithm_322213836_212054837::ThreatInfo TankAlgorithm_322213836_212054837::detectThreat(const Position &myPos) const {
        ThreatInfo threat;
        const int radius = 4;

        for (const auto &[shellPos, shellDir]: playerState.shellInfo) {
            Position pos = shellPos;
            int steps = 0;

            while (steps < radius * 2) {
                if (pos == myPos) {
                    threat.exists = true;
                    threat.pos = shellPos;
                    threat.dir = shellDir;
                    threat.steps = steps;
                    break;
                }
                pos = wrap(pos + DirectionUtils::dirToVector(shellDir), mapWidth, mapHeight);
                if (isWall(playerState.latestMap, pos)) break;
                steps++;
            }
        }

        return threat;
    }

    bool TankAlgorithm_322213836_212054837::canMoveForward(Position myPos, Direction myDir) {
        Position forward = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
        if (isWalkable(forward, playerState.latestMap) && !isTank(playerState.latestMap[forward.x][forward.y])) {
            playerState.myTanksInfo[tankId].first = forward;
            return true;
        }
        return false;
    }

    ActionRequest
    TankAlgorithm_322213836_212054837::evadeThreat(const ThreatInfo &threat, Position myPos, Direction myDir) {
        Direction oppDir = DirectionUtils::getOppositeDirection(threat.dir);
        if (myDir == oppDir && canShoot()) {
            return shoot();
        }
        if (threat.dir != myDir && oppDir != myDir) {
            if (canMoveForward(myPos, myDir)) {
                return ActionRequest::MoveForward;
            }
        }
//        Position forward = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
//        if (isWalkable(forward)) {
//            playerState.myTanksInfo[tankId].first = forward;
//            return ActionRequest::MoveForward;
//        }
//    }

        for (Direction dir: getSafeRotations(myPos)) {
            // If we're already facing a safe direction, try moving forward first
            if (myDir == dir) {
                if (canMoveForward(myPos, myDir)) {
                    return ActionRequest::MoveForward;
                }
                // If we can't move forward, continue to find another safe direction
                continue;
            }
            return rotateToward(myDir, dir);
        }

        for (Direction dir: getSafeMoves(myPos)) {
            // If we're already facing a safe direction, try moving forward first
            if (myDir == dir) {
                if (canMoveForward(myPos, myDir)) {
                    return ActionRequest::MoveForward;
                }
                // If we can't move forward, continue to find another safe direction
                continue;
            }
            return rotateToward(myDir, dir);
        }

        if (canShoot()) {
            return shoot();
        }

        return ActionRequest::GetBattleInfo;
    }

    ActionRequest TankAlgorithm_322213836_212054837::chaseEnemy(Position myPos, Direction myDir) {
        auto enemy = getNearestEnemy(myPos);
        if (!enemy.has_value()) return ActionRequest::GetBattleInfo;

        Position enemyPos = enemy.value();
        if (shouldRecalculatePath(enemyPos)) {
            bfsPath = bfsToEnemy(myPos, enemyPos, playerState.latestMap);
            lastEnemyPos = enemyPos;
        }

        if (canShootEnemy(myDir, myPos, enemyPos)) {
            return shoot();
        }

        if (bfsPath.size() < 7) {
            for (Direction dir: getAllDirections()) {
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
            if (canShoot()) {
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
            for (Direction dir: getSafeRotations(myPos)) {
                bfsToEnemy(myPos, enemyPos, playerState.latestMap);
                // If we're already facing a safe direction, try moving forward first
                if (myDir == dir) {
                    if (canMoveForward(myPos, myDir)) {
                        return ActionRequest::MoveForward;
                    }
                    // If we can't move forward, continue to find another safe direction
                    continue;
                }
                return rotateToward(myDir, dir);
            }
            return ActionRequest::GetBattleInfo;
        }

        bfsPath.erase(bfsPath.begin());
        if (canMoveForward(myPos, myDir)) {
            return ActionRequest::MoveForward;
        }

        // If we can't move forward and none of the above conditions were met,
        // try to find a safe direction to rotate to
        for (Direction dir: getSafeRotations(myPos)) {
            if (myDir == dir) {
                continue;  // Skip if we're already facing this direction
            }
            return rotateToward(myDir, dir);
        }

        // If we can't find any safe moves, get new battle info
        return ActionRequest::GetBattleInfo;
    }

    bool TankAlgorithm_322213836_212054837::canShoot() const {
        return (tankState.roundCounter - lastShotRound > SHOOTING_COOLDOWN) && tankState.remainingShells > 0;
    }

    ActionRequest TankAlgorithm_322213836_212054837::shoot() {
        tankState.remainingShells--;
        lastShotRound = tankState.roundCounter;
        return ActionRequest::Shoot;
    }


    void TankAlgorithm_322213836_212054837::updateShellPositions() {
        if (playerState.latestMap.size() == 0) {
            return;
        }
        // First clear all shells from the map
        for (size_t x = 0; x < mapWidth; ++x) {
            for (size_t y = 0; y < mapHeight; ++y) {
                if (playerState.latestMap[x][y] == '*') {
                    playerState.latestMap[x][y] = EMPTY;
                }
            }
        }

        // Update shell positions and map
        std::unordered_map<Position, Direction> updatedShells;
        for (const auto &[oldPos, shellDir]: playerState.shellInfo) {
            // Calculate new position (2 cells in shell's direction)
            Position dirVector = DirectionUtils::dirToVector(shellDir);
            int newX = (oldPos.x + 2 * dirVector.x + static_cast<int>(mapWidth)) % static_cast<int>(mapWidth);
            int newY = (oldPos.y + 2 * dirVector.y + static_cast<int>(mapHeight)) % static_cast<int>(mapHeight);
            Position newPos{newX, newY};

            // Only keep shell if it hasn't hit a wall
            if (!isWall(playerState.latestMap, newPos)) {
                updatedShells[newPos] = shellDir;
                // Update the map with the new shell position
                playerState.latestMap[newPos.x][newPos.y] = EMPTY;
            }
        }
        // Update shell info with the new positions
        playerState.shellInfo = std::move(updatedShells);
    }

    std::optional<Position> TankAlgorithm_322213836_212054837::getNearestEnemy(const Position &myPos) {
        // Find the nearest enemy
        Position nearestPos;
        int minDistance = std::numeric_limits<int>::max();
        bool foundEnemy = false;

        for (const auto &[pos, _]: playerState.enemyTanksInfo) {
            int distance = std::abs(pos.x - myPos.x) + std::abs(pos.y - myPos.y);
            if (distance < minDistance) {
                minDistance = distance;
                nearestPos = pos;
                foundEnemy = true;
            }
        }

        return foundEnemy ? std::optional<Position>(nearestPos) : std::nullopt;
    }

    bool TankAlgorithm_322213836_212054837::shouldRecalculatePath(const Position &enemyPos) const {
        return bfsPath.empty() || lastEnemyPos != enemyPos || pathBlocked(bfsPath);
    }

    bool TankAlgorithm_322213836_212054837::pathBlocked(const std::vector<Position> &path) const {
        for (const auto &pos: path) {
            if (!isWalkable(pos, playerState.latestMap))
                return true;
        }
        return false;
    }

    std::vector<Direction> TankAlgorithm_322213836_212054837::getSafeRotations(const Position &myPos) const {
        std::vector<Direction> safeDirs;
        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
            int d = (static_cast<int>(tankState.tankDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
            Direction dir = static_cast<Direction>(d);
            Position pos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (isWalkable(pos, playerState.latestMap)) {
                safeDirs.push_back(dir);
            }
        }
        return safeDirs;
    }

    std::vector<Direction> TankAlgorithm_322213836_212054837::getSafeMoves(const Position &myPos) const {
        std::vector<Direction> moves;
        for (int d = 0; d < NUM_DIRECTIONS; ++d) {
            Direction dir = static_cast<Direction>(d);
            Position next = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (isWalkable(next, playerState.latestMap)) {
                moves.push_back(dir);
            }
        }
        return moves;
    }

    std::vector<Direction> TankAlgorithm_322213836_212054837::getAllDirections() const {
        std::vector<Direction> dirs;
        for (int d = 0; d < NUM_DIRECTIONS; ++d) {
            dirs.push_back(static_cast<Direction>(d));
        }
        return dirs;
    }

    bool TankAlgorithm_322213836_212054837::canShootEnemy(Direction dir, const Position &myPos,
                                                          const Position &enemyPos) const {
        if (!canShoot()) return false;

        Position current = myPos;
        int steps = 0;
        const int maxSteps = std::max(mapWidth, mapHeight); // or a hard cap like 20

        while (steps++ < maxSteps) {
            current = wrap(current + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (current == enemyPos) return true;
            if (isWall(playerState.latestMap, current)) return false;
        }

        return false; // didn't hit anything, assume can't shoot
    }

    void TankAlgorithm_322213836_212054837::updateBattleInfo(BattleInfo &info) {
        // Get a copy of all the data we need before doing any dynamic casting
        auto *concreteInfo = dynamic_cast<ConcreteBattleInfo *>(&info);

        auto playerUpdates = concreteInfo->getPlayerUpdates();

        // Now we can safely cast and update our state

        if (concreteInfo) {
            // Update our state with the copies we made
            playerState = playerUpdates;
            concreteInfo->setTankUpdates(tankState);
            numTanks = playerState.myTanksInfo.size();
            if (tankState.remainingShells == UNINITIALIZED) {
                tankState.remainingShells = playerState.initShells;
            }
            // Update map dimensions only the first time
            if (mapWidth == static_cast<size_t>(UNINITIALIZED) && !playerState.latestMap.empty()) {
                mapWidth = playerState.latestMap.size();
                mapHeight = playerState.latestMap[0].size();
            }
        }
    }
}