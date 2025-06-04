#include "game/AnalyticalPlayer.h"
#include "game/ConcreteBattleInfo.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"
#include "common/BattleInfo.h"
#include "game/ConcreteTankAlgorithm.h"
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>
#include <limits>
#include <queue>
#include <algorithm>

AnalyticalPlayer::AnalyticalPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : Player(playerIndex, x, y, maxSteps, numShells),
      playerId(playerIndex), boardWidth(x), boardHeight(y), maxSteps(maxSteps), numShells(numShells) {
    // Initialize game state
    gameState.latestMap.resize(x, std::vector<char>(y, ' '));
    gameState.initShells = numShells;
}

void AnalyticalPlayer::initializeTanksInfo(int numTanks) {
    gameState.myTanksInfo.resize(numTanks, {Position{0,0}, Direction::R});
    for (int i = 0; i < numTanks; ++i) {
        TankState state;
        state.tankId = i;
        state.remainingShells = numShells;
        state.lastShotRound = -4;  // Initialize to allow immediate shooting
        tankStates[i] = state;
    }
}

void AnalyticalPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // Create battle info with current state
    ConcreteBattleInfo::PlayerUpdates updates;
    updates.myTanksInfo = gameState.myTanksInfo;
    updates.enemyTanksInfo = gameState.enemyTanksInfo;
    updates.latestMap = gameState.latestMap;
    updates.shellInfo = gameState.shellInfo;
    updates.turnId = gameState.turnId;
    updates.initShells = gameState.initShells;

    auto battleInfo = std::make_unique<ConcreteBattleInfo>(updates);
    
    // Update tank with battle info
    tank.updateBattleInfo(*battleInfo);

    // Get updated tank state
    auto tankUpdates = battleInfo->getTankUpdates();
    
    // Update our game state with new information
    for (const auto& [tankId, tankState] : tankUpdates) {
        if (tankId == playerId) {
            gameState.myTanksInfo[tankId] = {tankState.pos, tankState.dir};
            if (tankStates.find(tankId) != tankStates.end()) {
                tankStates[tankId].pos = tankState.pos;
                tankStates[tankId].dir = tankState.dir;
                tankStates[tankId].remainingShells = tankState.remainingShells;
            }
        } else {
            gameState.enemyTanksInfo[tankId] = {tankState.pos, tankState.dir};
        }
    }

    // Update map from satellite view
    for (size_t x = 0; x < boardWidth; ++x) {
        for (size_t y = 0; y < boardHeight; ++y) {
            gameState.latestMap[x][y] = satellite_view.getObjectAt(x, y);
        }
    }

    // Perform strategic analysis
    updateDangerMap();
    predictShellMovements();
    analyzeDangerZones();
    analyzeStrategicPositions();
    updateTankStates();
    calculateAttackPaths();
}

void AnalyticalPlayer::updateDangerMap() {
    // Clear previous danger map
    gameState.dangerMap.clear();

    // Add danger from enemy tanks
    for (const auto& [pos, dir] : gameState.enemyTanksInfo) {
        // Add danger to positions in line of sight
        auto lineOfSight = getLineOfSight(pos, dir);
        for (const auto& sightPos : lineOfSight) {
            gameState.dangerMap[sightPos] += 2;  // Higher danger from direct line of sight
        }
        
        // Add danger to adjacent positions
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                Position adjPos = {
                    (pos.x + dx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
                    (pos.y + dy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
                };
                gameState.dangerMap[adjPos] += 1;
            }
        }
    }

    // Add danger from shells
    for (const auto& [pos, dir] : gameState.shellInfo) {
        Position current = pos;
        for (int i = 0; i < 3; ++i) {  // Look ahead 3 steps
            current = {
                (current.x + DirectionUtils::dirToVector(dir).x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
                (current.y + DirectionUtils::dirToVector(dir).y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
            };
            if (gameState.latestMap[current.x][current.y] == '#') break;  // Stop at walls
            gameState.dangerMap[current] += 3;  // High danger from shells
        }
    }
}

void AnalyticalPlayer::predictShellMovements() {
    std::unordered_map<Position, Direction> predictedShells;
    
    for (const auto& [pos, dir] : gameState.shellInfo) {
        Position nextPos = {
            (pos.x + 2 * DirectionUtils::dirToVector(dir).x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
            (pos.y + 2 * DirectionUtils::dirToVector(dir).y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
        };
        
        if (gameState.latestMap[nextPos.x][nextPos.y] != '#') {
            predictedShells[nextPos] = dir;
        }
    }
    
    gameState.shellInfo = std::move(predictedShells);
}

void AnalyticalPlayer::analyzeDangerZones() {
    strategy.dangerZones.clear();
    
    // Add positions with high danger to danger zones
    for (const auto& [pos, danger] : gameState.dangerMap) {
        if (danger >= 3) {  // Threshold for considering a position dangerous
            strategy.dangerZones.insert(pos);
        }
    }
    
    // Add positions near walls as danger zones (can be trapped)
    for (size_t x = 0; x < boardWidth; ++x) {
        for (size_t y = 0; y < boardHeight; ++y) {
            if (gameState.latestMap[x][y] == '#') {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;
                        Position adjPos = {
                            (static_cast<int>(x) + dx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
                            (static_cast<int>(y) + dy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
                        };
                        strategy.dangerZones.insert(adjPos);
                    }
                }
            }
        }
    }
}

void AnalyticalPlayer::analyzeStrategicPositions() {
    strategy.highValuePositions.clear();
    gameState.valueMap.clear();
    
    // Calculate value for each position
    for (size_t x = 0; x < boardWidth; ++x) {
        for (size_t y = 0; y < boardHeight; ++y) {
            Position pos{static_cast<int>(x), static_cast<int>(y)};
            int value = calculatePositionValue(pos);
            gameState.valueMap[pos] = value;
            
            if (value >= 5) {  // Threshold for considering a position strategically valuable
                strategy.highValuePositions.insert(pos);
            }
        }
    }
}

int AnalyticalPlayer::calculatePositionValue(const Position& pos) const {
    int value = 0;
    
    // Check if position is safe
    if (gameState.dangerMap.find(pos) != gameState.dangerMap.end()) {
        value -= gameState.dangerMap.at(pos);
    }
    
    // Add value for positions that can attack multiple enemies
    for (const auto& [enemyPos, _] : gameState.enemyTanksInfo) {
        for (Direction dir : {Direction::R, Direction::L, Direction::U, Direction::D}) {
            auto lineOfSight = getLineOfSight(pos, dir);
            if (std::find(lineOfSight.begin(), lineOfSight.end(), enemyPos) != lineOfSight.end()) {
                value += 2;  // Position can attack an enemy
            }
        }
    }
    
    // Add value for positions that are hard to attack
    int wallsNearby = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            Position adjPos = {
                (pos.x + dx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
                (pos.y + dy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
            };
            if (gameState.latestMap[adjPos.x][adjPos.y] == '#') {
                wallsNearby++;
            }
        }
    }
    value += wallsNearby;  // Positions near walls are more defensible
    
    return value;
}

void AnalyticalPlayer::updateTankStates() {
    for (auto& [tankId, state] : tankStates) {
        // Update safe zones
        state.safeZones.clear();
        for (size_t x = 0; x < boardWidth; ++x) {
            for (size_t y = 0; y < boardHeight; ++y) {
                Position pos{static_cast<int>(x), static_cast<int>(y)};
                if (isPositionSafe(pos, tankId)) {
                    state.safeZones.push_back(pos);
                }
            }
        }
        
        // Update attack zones
        state.attackZones.clear();
        for (const auto& [enemyPos, _] : gameState.enemyTanksInfo) {
            for (Direction dir : {Direction::R, Direction::L, Direction::U, Direction::D}) {
                auto lineOfSight = getLineOfSight(state.pos, dir);
                if (std::find(lineOfSight.begin(), lineOfSight.end(), enemyPos) != lineOfSight.end()) {
                    state.attackZones.push_back(state.pos);
                    break;
                }
            }
        }
    }
}

void AnalyticalPlayer::calculateAttackPaths() {
    strategy.attackPaths.clear();
    
    for (const auto& [tankId, state] : tankStates) {
        for (const auto& [enemyPos, _] : gameState.enemyTanksInfo) {
            auto path = findSafePath(state.pos, enemyPos, tankId);
            if (!path.empty()) {
                strategy.attackPaths[enemyPos] = path;
            }
        }
    }
}

bool AnalyticalPlayer::isPositionSafe(const Position& pos, int tankId) const {
    // Check if position is in danger zones
    if (strategy.dangerZones.find(pos) != strategy.dangerZones.end()) {
        return false;
    }
    
    // Check if position is walkable
    if (gameState.latestMap[pos.x][pos.y] != ' ') {
        return false;
    }
    
    // Check if position is too close to enemy tanks
    for (const auto& [enemyPos, _] : gameState.enemyTanksInfo) {
        if (std::abs(pos.x - enemyPos.x) + std::abs(pos.y - enemyPos.y) <= 1) {
            return false;
        }
    }
    
    return true;
}

std::vector<Position> AnalyticalPlayer::findSafePath(const Position& start, const Position& goal, int tankId) const {
    std::queue<std::pair<Position, std::vector<Position>>> q;
    std::unordered_set<std::string> visited;
    
    q.push({start, {start}});
    visited.insert(start.toString());
    
    while (!q.empty()) {
        auto [current, path] = q.front(); q.pop();
        
        if (current == goal) {
            return path;
        }
        
        for (Direction dir : {Direction::R, Direction::L, Direction::U, Direction::D}) {
            Position next = {
                (current.x + DirectionUtils::dirToVector(dir).x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
                (current.y + DirectionUtils::dirToVector(dir).y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
            };
            
            if (visited.count(next.toString()) == 0 && isPositionSafe(next, tankId)) {
                visited.insert(next.toString());
                auto newPath = path;
                newPath.push_back(next);
                q.push({next, newPath});
            }
        }
    }
    
    return {};  // No safe path found
}

std::vector<Position> AnalyticalPlayer::getLineOfSight(const Position& start, Direction dir) const {
    std::vector<Position> line;
    Position current = start;
    
    while (true) {
        current = {
            (current.x + DirectionUtils::dirToVector(dir).x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth),
            (current.y + DirectionUtils::dirToVector(dir).y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight)
        };
        
        if (gameState.latestMap[current.x][current.y] == '#') {
            break;  // Stop at walls
        }
        
        line.push_back(current);
    }
    
    return line;
} 