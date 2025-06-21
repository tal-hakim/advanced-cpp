#include "implementations/PlayerTwo.h"
#include <iostream>
#include <queue>
#include <unordered_set>
#include <algorithm>

using namespace MapUtils;

// Maximum Manhattan distance to consider for initial enemy assignment
constexpr int ASSIGNMENT_RADIUS = 10;  // Adjust this value based on map size and gameplay needs

// Helper function to check if a tank is dead
bool PlayerTwo::isTankDead(int tankId) {
    // A tank is dead if it doesn't appear in tankTurns
    return tankTurns.find(tankId) == tankTurns.end();
}

// Helper function to find best enemy assignment for a specific tank
std::pair<int, std::vector<Position>> findBestEnemyAssignment(int tankId, const Position& myPos, 
                                                            const std::vector<std::pair<Position, Direction>>& enemyTanks,
                                                            const std::vector<std::vector<char>>& map,
                                                            const std::unordered_map<int, int>& currentAssignments) {
    int closestEnemyIdx = -1;
    size_t minPathLength = std::numeric_limits<size_t>::max();
    std::vector<Position> bestPath;

    // First pass: Find alive enemies within radius using Manhattan distance
    std::vector<std::pair<int, int>> nearbyEnemies;  // {enemyIdx, manhattanDistance}
    for (size_t enemyIdx = 0; enemyIdx < enemyTanks.size(); ++enemyIdx) {
        // Skip if this enemy is dead
        if (isEnemyTankDead(enemyIdx, enemyTanks)) {
            continue;
        }

        // Skip if this enemy is already assigned to another tank
        bool isAssigned = false;
        for (const auto& [assignedTankId, assignedEnemyIdx] : currentAssignments) {
            if (assignedEnemyIdx == static_cast<int>(enemyIdx) && assignedTankId != tankId) {
                isAssigned = true;
                break;
            }
        }
        if (isAssigned) continue;

        // Calculate Manhattan distance
        Position enemyPos = enemyTanks[enemyIdx].first;
        int distance = manhattanDistance(myPos, enemyPos);
        
        // Only consider enemies within radius
        if (distance <= ASSIGNMENT_RADIUS) {
            nearbyEnemies.emplace_back(enemyIdx, distance);
        }
    }

    // Second pass: Calculate BFS paths only for nearby enemies
    for (const auto& [enemyIdx, _] : nearbyEnemies) {
        Position enemyPos = enemyTanks[enemyIdx].first;
        auto path = bfsToEnemy(myPos, enemyPos, map);
        
        // If we found a valid path and it's shorter than our current best
        if (!path.empty() && path.size() < minPathLength) {
            minPathLength = path.size();
            closestEnemyIdx = enemyIdx;
            bestPath = std::move(path);
        }
    }

    // If no nearby enemies were found or none had valid paths, 
    // fall back to the closest unassigned alive enemy by Manhattan distance
    if (closestEnemyIdx == -1) {
        int minDistance = std::numeric_limits<int>::max();
        for (size_t enemyIdx = 0; enemyIdx < enemyTanks.size(); ++enemyIdx) {
            // Skip if this enemy is dead
            if (isEnemyTankDead(enemyIdx, enemyTanks)) {
                continue;
            }

            // Skip if this enemy is already assigned
            bool isAssigned = false;
            for (const auto& [assignedTankId, assignedEnemyIdx] : currentAssignments) {
                if (assignedEnemyIdx == static_cast<int>(enemyIdx) && assignedTankId != tankId) {
                    isAssigned = true;
                    break;
                }
            }
            if (isAssigned) continue;

            Position enemyPos = enemyTanks[enemyIdx].first;
            int distance = manhattanDistance(myPos, enemyPos);
            if (distance < minDistance) {
                minDistance = distance;
                closestEnemyIdx = enemyIdx;
                bestPath = bfsToEnemy(myPos, enemyPos, map);
            }
        }
    }

    return {closestEnemyIdx, bestPath};
}

void PlayerTwo::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // Get tank ID and update board state
    auto* tankPtr = dynamic_cast<ConcreteTankAlgorithm*>(&tank);
    int tankId = tankPtr->getTankId();
    
    analyzeBoard(tankId, satellite_view);
    handleTankTurnReordering(tankId);
    playerState.turnId = tankTurns[tankId];
    
    // Clean up dead tanks from assignments
    for (auto it = tankAssignments.begin(); it != tankAssignments.end();) {
        // Remove assignment if either tank is dead
        if (isTankDead(it->first) ||
            isEnemyTankDead(it->second, playerState.enemyTanksInfo)) {
            tankPaths.erase(it->first);
            lastEnemyPositions.erase(it->first);
            it = tankAssignments.erase(it);
        } else {
            ++it;
        }
    }
    
    // Create battle info with current player state
    ConcreteBattleInfo info(playerState);
    
    // Get current position of the tank requesting battle info
    Position myPos = playerState.myTanksInfo[tankId].first;
    
    // Check if we need to update this tank's assignment
    bool needsNewAssignment = tankAssignments.find(tankId) == tankAssignments.end();
    
    if (needsNewAssignment) {
        // Find best enemy assignment for this specific tank
        auto [enemyIdx, path] = findBestEnemyAssignment(tankId, myPos, playerState.enemyTanksInfo, 
                                                      playerState.latestMap, tankAssignments);
        
        if (enemyIdx != -1) {
            // Update assignment and path for this tank only
            tankAssignments[tankId] = enemyIdx;
            lastEnemyPositions[tankId] = playerState.enemyTanksInfo[enemyIdx].first;
            tankPaths[tankId] = std::move(path);
        }
    } else {
        // If we have a valid assignment, check if we need to update the path
        Position enemyPos = playerState.enemyTanksInfo[tankAssignments[tankId]].first;
        if (shouldRecalculatePath(tankId, enemyPos)) {
            tankPaths[tankId] = bfsToEnemy(myPos, enemyPos, playerState.latestMap);
            lastEnemyPositions[tankId] = enemyPos;
        }
    }
    
    // Set enemy info and path in battle info if we have a valid assignment
    auto assignmentIt = tankAssignments.find(tankId);
    if (assignmentIt != tankAssignments.end() && 
        assignmentIt->second < static_cast<int>(playerState.enemyTanksInfo.size())) {
        info.setEnemy(playerState.enemyTanksInfo[assignmentIt->second]);
        auto updates = info.getPlayerUpdates();
        updates.bfsToEnemy = tankPaths[tankId];
        info.setPlayerUpdates(updates);
    }
    
    // Update tank with battle info
    tank.updateBattleInfo(info);
    ConcreteBattleInfo::TankUpdates tankUpdates = info.getTankUpdates();
    
    // Update tank state
    remainingShells[tankId] = tankUpdates.remainingShells;
    roundCounter = tankUpdates.roundCounter;
    playerState.myTanksInfo[tankId].second = tankUpdates.tankDir;
}

bool PlayerTwo::shouldRecalculatePath(int tankId, const Position& enemyPos) const {
    // Get the tank's current path and last known enemy position
    auto pathIt = tankPaths.find(tankId);
    auto lastPosIt = lastEnemyPositions.find(tankId);
    
    // If we don't have a path or last position, we need to recalculate
    if (pathIt == tankPaths.end() || lastPosIt == lastEnemyPositions.end()) {
        return true;
    }
    
    // If enemy has moved, we need to recalculate
    if (lastPosIt->second != enemyPos) {
        return true;
    }
    
    
    return false;
}

