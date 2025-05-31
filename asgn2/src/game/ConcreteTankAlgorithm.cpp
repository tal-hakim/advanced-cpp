#include "game/ConcreteTankAlgorithm.h"
#include <queue>
#include <limits>

// Helper function to determine rotation action
static ActionRequest rotateToward(Direction current, Direction target) {
    if (current == target) return ActionRequest::DoNothing;
    
    int currentAngle = static_cast<int>(current);
    int targetAngle = static_cast<int>(target);
    int diff = (targetAngle - currentAngle + 360) % 360;
    
    if (diff == 45) return ActionRequest::RotateRight45;
    if (diff == 90) return ActionRequest::RotateRight90;
    if (diff == 135) return ActionRequest::RotateRight45;
    if (diff == 180) return ActionRequest::RotateRight90;
    if (diff == 225) return ActionRequest::RotateLeft45;
    if (diff == 270) return ActionRequest::RotateLeft90;
    if (diff == 315) return ActionRequest::RotateLeft45;
    
    return ActionRequest::DoNothing;
}

// Helper functions to work directly with map data
bool isWall(const std::vector<std::vector<char>>& map, const Position& pos) {
    return map[pos.y][pos.x] == '#';
}

bool isMine(const std::vector<std::vector<char>>& map, const Position& pos) {
    return map[pos.y][pos.x] == '*';
}

bool isTank(const std::vector<std::vector<char>>& map, const Position& pos) {
    char c = map[pos.y][pos.x];
    return c >= '1' && c <= '9';
}

Position wrap(const Position& pos, size_t width, size_t height) {
    int x = (pos.x + width) % width;
    int y = (pos.y + height) % height;
    return Position{x, y};
}


ActionRequest ConcreteTankAlgorithm::getAction() {
    tankState.roundCounter++;
    // First check if we need to get battle info
    std::cout << "Debug - Round: " << tankState.roundCounter 
              << ", Tanks: " << static_cast<int>(numTanks)
              << ", Turn Id: " << playerState.turnId << std::endl;
    if(tankState.roundCounter == 0 || tankState.roundCounter % static_cast<int>(numTanks) == playerState.turnId) {
        return ActionRequest::GetBattleInfo;
    }

    // Get map dimensions
    size_t mapWidth = playerState.latestMap[0].size();
    size_t mapHeight = playerState.latestMap.size();

    // Get my tank info
    Position myPos = playerState.myTanksInfo[tankId].first;
    Direction myDir = playerState.myTanksInfo[tankId].second;
    int myShells = tankState.remainingShells;

    // Check for threats using Evader's logic FIRST
    const int radius = 4;
    int closestThreatSteps = INT_MAX;
    Position closestThreatPos;
    Direction closestThreatDir;

    // Check for shells in radius
    for (const auto& [shellPos, shellDir] : playerState.shellInfo) {
        // Calculate steps until shell hits tank
        Position currentShellPos = shellPos;
        Direction currentShellDir = shellDir;
        int steps = 0;
        bool hit = false;

        while (steps < radius * 2) {  // Limit search radius
            if (currentShellPos == myPos) {
                hit = true;
                break;
            }
            currentShellPos = wrap(currentShellPos + DirectionUtils::dirToVector(currentShellDir), mapWidth, mapHeight);
            if (isWall(playerState.latestMap, currentShellPos)) {
                break;
            }
            steps++;
        }

        if (hit && steps < closestThreatSteps) {
            closestThreatSteps = steps;
            closestThreatPos = shellPos;
            closestThreatDir = shellDir;
        }
    }

    // If we have a threat, use Evader logic immediately
    if (closestThreatSteps != INT_MAX) {
        // Defensive fire if directly aligned and not in cooldown
        if (myDir == closestThreatDir && myShells > 0 && 
            tankState.roundCounter - lastShotRound >= 4) {  // Check cooldown
            tankState.remainingShells--;  // Update remaining shells
            lastShotRound = tankState.roundCounter;  // Update last shot round
            return ActionRequest::Shoot;
        }

        Direction shellDir = closestThreatDir;
        Direction shellOpposite = DirectionUtils::getOppositeDirection(shellDir);

        // Try to move forward if not in shell's path
        if (shellDir != myDir && shellOpposite != myDir) {
            Position forwardPos = wrap(myPos + DirectionUtils::dirToVector(myDir), mapWidth, mapHeight);
            if (!isWall(playerState.latestMap, forwardPos) && 
                !isMine(playerState.latestMap, forwardPos) && 
                !isTank(playerState.latestMap, forwardPos)) {
                // Update position in player state
                playerState.myTanksInfo[tankId].first = forwardPos;
                return ActionRequest::MoveForward;
            }
        }

        // Try to rotate away from threat
        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
            int d = (static_cast<int>(myDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
            Direction dir = static_cast<Direction>(d);
            Position tmpPos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (!isWall(playerState.latestMap, tmpPos) && !isMine(playerState.latestMap, tmpPos)) {
                // Update direction in player state
                playerState.myTanksInfo[tankId].second = dir;
                return rotateToward(myDir, dir);
            }
        }

        // If we can't find an optimal move, try ANY safe move
        for (int d = 0; d < NUM_DIRECTIONS; ++d) {
            Direction dir = static_cast<Direction>(d);
            Position nextPos = wrap(myPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (!isWall(playerState.latestMap, nextPos) && 
                !isMine(playerState.latestMap, nextPos) && 
                !isTank(playerState.latestMap, nextPos)) {
                // Found a safe move, rotate towards it
                playerState.myTanksInfo[tankId].second = dir;
                return rotateToward(myDir, dir);
            }
        }

        // If we still can't find a safe move, try to shoot in any direction
        if (myShells > 0 && tankState.roundCounter - lastShotRound >= 4) {  // Check cooldown
            tankState.remainingShells--;
            lastShotRound = tankState.roundCounter;  // Update last shot round
            return ActionRequest::Shoot;
        }

        // Only if we have no shells and no safe moves, get battle info
        return ActionRequest::GetBattleInfo;
    }

    // Only look for enemies if we're not in immediate danger
    Position enemyPos{0,0};
    bool foundOpponent = false;
    for (const auto& [pos, dir] : playerState.enemyTanksInfo) {
        if (!foundOpponent) {
            enemyPos = pos;
            foundOpponent = true;
        }
    }

    if (!foundOpponent) {
        return ActionRequest::GetBattleInfo;
    }

    // No threats, use Chaser logic
    auto bfsToEnemy = [&](const Position& start, const Position& goal) {
        std::queue<std::pair<Position, std::vector<Position>>> q;
        std::unordered_set<std::string> visited;

        q.push({start, {start}});
        visited.insert(start.toString());

        while (!q.empty()) {
            auto [current, path] = q.front(); q.pop();

            if (current == goal) return path;

            for (int d = 0; d < NUM_DIRECTIONS; ++d) {
                Direction dir = static_cast<Direction>(d);
                Position next = wrap(current + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);

                if (!isWall(playerState.latestMap, next) &&
                    !isMine(playerState.latestMap, next) &&
                    visited.count(next.toString()) == 0) {

                    visited.insert(next.toString());
                    auto newPath = path;
                    newPath.push_back(next);
                    q.push({next, newPath});
                }
            }
        }

        return std::vector<Position>{};
    };

    auto pathBlocked = [&](const std::vector<Position>& path) {
        for (const auto& pos : path) {
            if (isWall(playerState.latestMap, pos) || isMine(playerState.latestMap, pos))
                return true;
        }
        return false;
    };

    bool shouldRecalculate = bfsPath.empty() ||
                            lastEnemyPos != enemyPos ||
                            pathBlocked(bfsPath);

    if (shouldRecalculate) {
        bfsPath = bfsToEnemy(myPos, enemyPos);
        lastEnemyPos = enemyPos;
    }

    auto canShootEnemy = [&](Direction dir) -> bool {
        if (myShells <= 0) return false;
        
        Position currentPos = myPos;
        while (true) {
            currentPos = wrap(currentPos + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);
            if (currentPos == enemyPos) return true;
            if (isWall(playerState.latestMap, currentPos)) return false;
        }
    };

    if (canShootEnemy(myDir) && tankState.roundCounter - lastShotRound >= 4) {  // Check cooldown
        tankState.remainingShells--;  // Update remaining shells
        lastShotRound = tankState.roundCounter;  // Update last shot round
        return ActionRequest::Shoot;
    }

    if (bfsPath.size() < 7) {
        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
            int d = (static_cast<int>(myDir) + i + NUM_DIRECTIONS) % NUM_DIRECTIONS;
            Direction dir = static_cast<Direction>(d);
            if (canShootEnemy(dir)) {
                if (i != 0) {
                    // Update direction in player state
                    playerState.myTanksInfo[tankId].second = dir;
                    return rotateToward(myDir, dir);
                }
            }
        }
    }

    if (bfsPath.size() < 4 && tankState.roundCounter - lastShotRound >= 4) {  // Check cooldown
        tankState.remainingShells--;  // Update remaining shells
        lastShotRound = tankState.roundCounter;  // Update last shot round
        return ActionRequest::Shoot;
    }

    Position next = bfsPath[1];
    Direction desiredDir = DirectionUtils::directionFromTo(myPos, next);
    
    if (myDir != desiredDir) {
        // Update direction in player state
        playerState.myTanksInfo[tankId].second = desiredDir;
        return rotateToward(myDir, desiredDir);
    }
    
    // Update position in player state for move forward
    playerState.myTanksInfo[tankId].first = next;
    bfsPath.erase(bfsPath.begin());
    return ActionRequest::MoveForward;
}


void ConcreteTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    auto* concreteInfo = dynamic_cast<ConcreteBattleInfo*>(&info);
    if (concreteInfo) {
        playerState = concreteInfo->getPlayerUpdates();
        concreteInfo->setTankUpdates(tankState);
        numTanks = playerState.myTanksInfo.size();
    }
}


