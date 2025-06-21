#include "implementations/ConcreteTankAlgorithm.h"
#include <queue>
#include <limits>
#include <optional>
#include "definitions.h"
#include "utils/MapUtils.h"
using namespace MapUtils;
// Helper function to determine rotation action
ActionRequest ConcreteTankAlgorithm::rotateToward(Direction current, Direction target) {
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



bool isTank(const std::vector<std::vector<char>>& map, const Position& pos) {
    char c = map[pos.x][pos.y];
    return c >= '1' && c <= '9';
}




// ====== Public Method ======

// ====== Top-Level Helpers ======
bool ConcreteTankAlgorithm::shouldGetBattleInfo() const {
    return ((tankState.roundCounter == 0 ||
            (tankState.roundCounter % static_cast<int>(numTanks) == playerState.turnId))
           && numTanks != 1) || (numTanks == 1 && tankState.roundCounter % 3 == 0);
}

ConcreteTankAlgorithm::ThreatInfo ConcreteTankAlgorithm::detectThreat(const Position& myPos) const {
    ThreatInfo threat;
    const int radius = 4;

    for (const auto& [shellPos, shellDir] : playerState.shellInfo) {
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

bool ConcreteTankAlgorithm::canMoveForward(Position myPos, Direction myDir){
    Position forward = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
    if (isWalkable(forward, playerState.latestMap) && !isTank(playerState.latestMap[forward.x][forward.y])) {
        playerState.myTanksInfo[tankId].first = forward;
        return true;
    }
    return false;
}

ActionRequest ConcreteTankAlgorithm::evadeThreat(const ThreatInfo &threat, Position myPos, Direction myDir)
{
    Direction oppDir = DirectionUtils::getOppositeDirection(threat.dir);
    if (myDir == oppDir && canShoot()) {
        return shoot();
    }
    if (threat.dir != myDir && oppDir != myDir) {
        if(canMoveForward(myPos, myDir)) {
            return ActionRequest::MoveForward;
        }
    }
//        Position forward = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
//        if (isWalkable(forward)) {
//            playerState.myTanksInfo[tankId].first = forward;
//            return ActionRequest::MoveForward;
//        }
//    }

    for (Direction dir : getSafeRotations(myPos)) {
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

    for (Direction dir : getSafeMoves(myPos)) {
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

ActionRequest ConcreteTankAlgorithm::chaseEnemy(Position myPos, Direction myDir) {
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
        for (Direction dir : getSafeRotations(myPos)) {
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
    if(canMoveForward(myPos, myDir)) {
        return ActionRequest::MoveForward;
    }
    
    // If we can't move forward and none of the above conditions were met,
    // try to find a safe direction to rotate to
    for (Direction dir : getSafeRotations(myPos)) {
        if (myDir == dir) {
            continue;  // Skip if we're already facing this direction
        }
        return rotateToward(myDir, dir);
    }
    
    // If we can't find any safe moves, get new battle info
    return ActionRequest::GetBattleInfo;
}







//// ====== BFS and Support ======
//void ConcreteTankAlgorithm::bfsToEnemy(Position start, Position goal) {
//    // TODO: write player 2, fix logger
//    std::queue<std::pair<Position, std::vector<Position>>> q;
//    std::unordered_set<std::string> visited;
//
//    q.push({start, {start}});
//    visited.insert(start.toString());
//
//    while (!q.empty()) {
//        auto [current, path] = q.front(); q.pop();
//
//        if (current == goal) {
//            bfsPath = path;
//            return;
//        }
//
//        for (int d = 0; d < NUM_DIRECTIONS ; ++d) {
//            Direction dir = static_cast<Direction>(d);
//            Position next = wrap(current + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//
//            // Skip walls, mines, and previously visited positions
//            if (isWalkable(next) &&
//                visited.count(next.toString()) == 0) {
//
//                visited.insert(next.toString());
//                auto newPath = path;
//                newPath.push_back(next);
//                q.push({next, newPath});
//            }
//        }
//    }
//
//    bfsPath = std::vector<Position>{};
//}

bool ConcreteTankAlgorithm::canShoot() const {
    return (tankState.roundCounter - lastShotRound > SHOOTING_COOLDOWN) && tankState.remainingShells > 0;
}

ActionRequest ConcreteTankAlgorithm::shoot() {
    tankState.remainingShells--;
    lastShotRound = tankState.roundCounter;
    return ActionRequest::Shoot;
}


void ConcreteTankAlgorithm::updateShellPositions() {
    if (playerState.latestMap.size() == 0){
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
    for (const auto& [oldPos, shellDir] : playerState.shellInfo) {
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

std::optional<Position> ConcreteTankAlgorithm::getNearestEnemy(const Position& myPos) {
    // Find the nearest enemy
    Position nearestPos;
    int minDistance = std::numeric_limits<int>::max();
    bool foundEnemy = false;

    for (const auto& [pos, _] : playerState.enemyTanksInfo) {
        int distance = std::abs(pos.x - myPos.x) + std::abs(pos.y - myPos.y);
        if (distance < minDistance) {
            minDistance = distance;
            nearestPos = pos;
            foundEnemy = true;
        }
    }

    return foundEnemy ? std::optional<Position>(nearestPos) : std::nullopt;
}

bool ConcreteTankAlgorithm::shouldRecalculatePath(const Position& enemyPos) const {
    return bfsPath.empty() || lastEnemyPos != enemyPos || pathBlocked(bfsPath);
}

bool ConcreteTankAlgorithm::pathBlocked(const std::vector<Position>& path) const {
    for (const auto& pos : path) {
        if (!isWalkable(pos, playerState.latestMap))
            return true;
    }
    return false;
}

std::vector<Direction> ConcreteTankAlgorithm::getSafeRotations(const Position& myPos) const {
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

std::vector<Direction> ConcreteTankAlgorithm::getSafeMoves(const Position& myPos) const {
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

std::vector<Direction> ConcreteTankAlgorithm::getAllDirections() const {
    std::vector<Direction> dirs;
    for (int d = 0; d < NUM_DIRECTIONS; ++d) {
        dirs.push_back(static_cast<Direction>(d));
    }
    return dirs;
}

bool ConcreteTankAlgorithm::canShootEnemy(Direction dir, const Position& myPos, const Position& enemyPos) const {
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



//ActionRequest ConcreteTankAlgorithm::getAction() {
//    tankState.roundCounter++;
//    // First check if we need to get battle info
//    std::cout << "Debug - Round: " << tankState.roundCounter
//              << ", Tanks: " << static_cast<int>(numTanks)
//              << ", Turn Id: " << playerState.turnId << std::endl;
//    if(tankState.roundCounter == 0 || tankState.roundCounter % static_cast<int>(numTanks) == playerState.turnId) {
//        if(numTanks != 1){
//            return ActionRequest::GetBattleInfo;
//        }
//    }
//
//    // Get my tank info
//    Position myPos = playerState.myTanksInfo[tankId].first;
//    Direction myDir = tankState.tankDir;
//    int myShells = tankState.remainingShells;
//
//    // Check for threats using Evader's logic FIRST
//    const int radius = 4;
//    int closestThreatSteps = INT_MAX;
//    Position closestThreatPos;
//    Direction closestThreatDir;
//
//    // Check for shells in radius
//    for (const auto& [shellPos, shellDir] : playerState.shellInfo) {
//        // Calculate steps until shell hits tank
//        Position currentShellPos = shellPos;
//        Direction currentShellDir = shellDir;
//        int steps = 0;
//        bool hit = false;
//
//        while (steps < radius * 2) {  // Limit search radius
//            if (currentShellPos == myPos) {
//                hit = true;
//                break;
//            }
//            currentShellPos = wrap(currentShellPos + DirectionUtils::dirToVector(currentShellDir), mapWidth, mapHeight);
//            if (isWall(playerState.latestMap, currentShellPos)) {
//                break;
//            }
//            steps++;
//        }
//
//        if (hit && steps < closestThreatSteps) {
//            closestThreatSteps = steps;
//            closestThreatPos = shellPos;
//            closestThreatDir = shellDir;
//        }
//    }
//
//    // If we have a threat, use Evader logic immediately
//    if (closestThreatSteps != INT_MAX) {
//        // Defensive fire if directly aligned and not in cooldown
//        if (myDir == closestThreatDir && myShells > 0 &&
//            tankState.roundCounter - lastShotRound >= SHOOTING_COOLDOWN) {  // Check cooldown
//            tankState.remainingShells--;  // Update remaining shells
//            lastShotRound = tankState.roundCounter;  // Update last shot round
//            return ActionRequest::Shoot;
//        }
//
//        Direction shellDir = closestThreatDir;
//        Direction shellOpposite = DirectionUtils::getOppositeDirection(shellDir);
//
//        // Try to move forward if not in shell's path
//        if (shellDir != myDir && shellOpposite != myDir) {
//            Position forwardPos = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
//            if (!isWall(playerState.latestMap, forwardPos) &&
//                !isMine(playerState.latestMap, forwardPos) &&
//                !isTank(playerState.latestMap, forwardPos)) {
//                // Update position in player state
//                playerState.myTanksInfo[tankId].first = forwardPos;
//                return ActionRequest::MoveForward;
//            }
//        }
//
//        // Try to rotate away from threat
//        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
//            int d = (static_cast<int>(myDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
//            Direction dir = static_cast<Direction>(d);
//            Position tmpPos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//            if (!isWall(playerState.latestMap, tmpPos) && !isMine(playerState.latestMap, tmpPos)) {
//                // Update direction in player state
//                tankState.tankDir = dir;
//                return rotateToward(myDir, dir);
//            }
//        }
//
//        // If we can't find an optimal move, try ANY safe move
//        for (int d = 0; d < NUM_DIRECTIONS; ++d) {
//            Direction dir = static_cast<Direction>(d);
//            Position nextPos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//            if (!isWall(playerState.latestMap, nextPos) &&
//                !isMine(playerState.latestMap, nextPos) &&
//                !isTank(playerState.latestMap, nextPos)) {
//                // Found a safe move, rotate towards it
//                tankState.tankDir = dir;
//                return rotateToward(myDir, dir);
//            }
//        }
//
//        // If we still can't find a safe move, try to shoot in any direction
//        if (myShells > 0 && tankState.roundCounter - lastShotRound >= SHOOTING_COOLDOWN) {  // Check cooldown
//            tankState.remainingShells--;
//            lastShotRound = tankState.roundCounter;  // Update last shot round
//            return ActionRequest::Shoot;
//        }
//
//        // Only if we have no shells and no safe moves, get battle info
//        return ActionRequest::GetBattleInfo;
//    }
//
//    // Only look for enemies if we're not in immediate danger
//    Position enemyPos{0,0};
//    bool foundOpponent = false;
//    for (const auto& [pos, dir] : playerState.enemyTanksInfo) {
//        if (!foundOpponent) {
//            enemyPos = pos;
//            foundOpponent = true;
//        }
//    }
//
//    if (!foundOpponent) {
//        return ActionRequest::GetBattleInfo;
//    }
//
//    // No threats, use Chaser logic
//    auto bfsToEnemy = [&](const Position& start, const Position& goal) {
//        std::queue<std::pair<Position, std::vector<Position>>> q;
//        std::unordered_set<std::string> visited;
//
//        q.push({start, {start}});
//        visited.insert(start.toString());
//
//        while (!q.empty()) {
//            auto [current, path] = q.front(); q.pop();
//
//            if (current == goal) return path;
//
//            for (int d = 0; d < NUM_DIRECTIONS; ++d) {
//                Direction dir = static_cast<Direction>(d);
//                Position next = wrap(current + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//
//                if (!isWall(playerState.latestMap, next) &&
//                    !isMine(playerState.latestMap, next) &&
//                    visited.count(next.toString()) == 0) {
//
//                    visited.insert(next.toString());
//                    auto newPath = path;
//                    newPath.push_back(next);
//                    q.push({next, newPath});
//                }
//            }
//        }
//
//        return std::vector<Position>{};
//    };
//
//    auto pathBlocked = [&](const std::vector<Position>& path) {
//        for (const auto& pos : path) {
//            if (isWall(playerState.latestMap, pos) || isMine(playerState.latestMap, pos))
//                return true;
//        }
//        return false;
//    };
//
//    bool shouldRecalculate = bfsPath.empty() ||
//                            lastEnemyPos != enemyPos ||
//                            pathBlocked(bfsPath);
//
//    if (shouldRecalculate) {
//        bfsPath = bfsToEnemy(myPos, enemyPos);
//        lastEnemyPos = enemyPos;
//    }
//
//    auto canShootEnemy = [&](Direction dir) -> bool {
//        if (myShells <= 0 || tankState.roundCounter - lastShotRound < SHOOTING_COOLDOWN) return false;
//
//        Position currentPos = myPos;
//        while (true) {
//            currentPos = wrap(currentPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//            if (currentPos == enemyPos) return true;
//            if (isWall(playerState.latestMap, currentPos)) return false;
//        }
//    };
//
//    // Only shoot if we can hit the enemy (cooldown is checked in canShootEnemy)
//    if (canShootEnemy(myDir)) {
//        tankState.remainingShells--;
//        lastShotRound = tankState.roundCounter;
//        return ActionRequest::Shoot;
//    }
//
//    // Try to rotate to a position where we can shoot
//    if (bfsPath.size() < 7) {
//        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
//            int d = (static_cast<int>(myDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
//            Direction dir = static_cast<Direction>(d);
//            if (canShootEnemy(dir)) {  // cooldown is checked in canShootEnemy
//                if (i != 0) {
//                    // Update direction in player state
//                    tankState.tankDir = dir;
//                    return rotateToward(myDir, dir);
//                }
//            }
//        }
//    }
//
//    // Don't shoot just because we're close - only shoot if we can hit
//    if (bfsPath.size() < 2) {
//        // If we're very close, try to rotate to face the enemy
//        Direction desiredDir = DirectionUtils::directionFromTo(myPos, enemyPos);
//        if (myDir != desiredDir) {
//            tankState.tankDir = desiredDir;
//            return rotateToward(myDir, desiredDir);
//        }
//        // Only get battle info if we're already facing the enemy
//        return ActionRequest::GetBattleInfo;
//    }
//
//    Position next = bfsPath[1];
//    Direction desiredDir = DirectionUtils::directionFromTo(myPos, next);
//
//    if (myDir != desiredDir) {
//        // Update direction in player state
//        tankState.tankDir = desiredDir;
//        return rotateToward(myDir, desiredDir);
//    }
//
//    // Check if next position is safe before moving
//    if (isWall(playerState.latestMap, next) || isMine(playerState.latestMap, next) || isTank(playerState.latestMap, next)) {
//        // Try to rotate to find a new path
//        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
//            int d = (static_cast<int>(myDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
//            Direction dir = static_cast<Direction>(d);
//            Position tmpPos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
//            if (!isWall(playerState.latestMap, tmpPos) && !isMine(playerState.latestMap, tmpPos) && !isTank(playerState.latestMap, tmpPos)) {
//                // Update direction in player state
//                tankState.tankDir = dir;
//                // Recalculate BFS path from new direction
//                bfsPath = bfsToEnemy(myPos, enemyPos);
//                return rotateToward(myDir, dir);
//            }
//        }
//        // If we can't find a safe rotation, get new battle info
//        return ActionRequest::GetBattleInfo;
//    }
//
//    // Update position in player state for move forward
//    playerState.myTanksInfo[tankId].first = next;
//    bfsPath.erase(bfsPath.begin());
//    return ActionRequest::MoveForward;
//}


void ConcreteTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Get a copy of all the data we need before doing any dynamic casting
    auto* concreteInfo = dynamic_cast<ConcreteBattleInfo*>(&info);

    auto playerUpdates = concreteInfo->getPlayerUpdates();

    // Now we can safely cast and update our state

    if (concreteInfo) {
        // Update our state with the copies we made
        playerState = playerUpdates;
        concreteInfo->setTankUpdates(tankState);
        numTanks = playerState.myTanksInfo.size();
        if(tankState.remainingShells == UNINITIALIZED){
            tankState.remainingShells = playerState.initShells;
        }
        // Update map dimensions only the first time
        if (mapWidth == static_cast<size_t>(UNINITIALIZED) && !playerState.latestMap.empty()) {
            mapWidth = playerState.latestMap.size();
            mapHeight = playerState.latestMap[0].size();
        }
    }
}


