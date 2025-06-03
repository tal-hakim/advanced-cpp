#include "game/ConcretePlayer.h"
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
#include <iostream>

ConcretePlayer::ConcretePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : Player(playerIndex, x, y, maxSteps, numShells),
      playerId(playerIndex), boardWidth(x), boardHeight(y), maxSteps(maxSteps), numShells(numShells),
      aliveTanks(UNINITIALIZED), reorderTurns(false) {
    // Initialize context with empty state
    playerState.latestMap.resize(x, std::vector<char>(y, ' '));
    playerState.initShells = numShells;
    // Don't initialize myTanksInfo - we'll resize it when we first see tanks
    
    roundCounter = 0;
}

void ConcretePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
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

std::vector<std::vector<char>> ConcretePlayer::readView(int tankId, SatelliteView& view){
    std::vector<std::vector<char>> map(boardWidth, std::vector<char>(boardHeight, ' '));
    int currAliveTanks = 0;
    
    // First pass: count tanks
    for(size_t i = 0; i < boardWidth ; i++){
        for (size_t j = 0; j <boardHeight; ++j) {
            char currObj = view.getObjectAt(i, j);
            if(currObj == '0' + playerId || currObj == '%') {
                currAliveTanks++;
            }
        }
    }
    
    // Initialize or resize myTanksInfo if needed
    if (this->aliveTanks == UNINITIALIZED) {
        playerState.myTanksInfo.resize(currAliveTanks, {Position{0,0}, Direction::R});
        this->aliveTanks = currAliveTanks;
        // Initialize tankTurns with sequential ordering (0:0, 1:1, etc.) when we first know the actual number of tanks
        if (tankTurns.empty()) {
            for (int i = 0; i < currAliveTanks; ++i) {
                tankTurns[i] = i;
            }
            reorderTurns = false;
        }
    }
    
    // Second pass: populate map and update positions
    for(size_t i = 0; i < boardWidth; i++){
        for (size_t j = 0; j < boardHeight; ++j) {
            char currObj = view.getObjectAt(i, j);
            if(currObj == CURR_TANK){
                playerState.myTanksInfo[tankId].first = Position{static_cast<int>(i), static_cast<int>(j)};
                map[i][j] = '0' + playerId;
            }
            else{
                map[i][j] = currObj;
            }
        }
    }
    
    if(currAliveTanks != this->aliveTanks){
        this->aliveTanks = currAliveTanks;
        reorderTurns = true;
        tankTurns.clear();
    }
    return map;
}

void ConcretePlayer::analyzeWindowAroundObject(const Position& pos, int windowSize, const std::vector<std::vector<char>>& map) {
    for (int dy = -windowSize; dy <= windowSize; ++dy) {
        for (int dx = -windowSize; dx <= windowSize; ++dx) {
            // Calculate wrapped position
            int wrappedX = (pos.x + dx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
            int wrappedY = (pos.y + dy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);
            Position windowPos{wrappedX, wrappedY};
            
            char obj = map[windowPos.x][windowPos.y];
            if (obj != ' ') {  // Only exclude empty space
                Direction dir = DirectionUtils::directionFromTo(pos, windowPos);
                
                if (obj == '*') {
                    // Check if this shell existed anywhere in a 5x5 area in the previous map
                    bool isNewShell = true;
                    if (!playerState.latestMap.empty()) {
                        for (int prevDy = -2; prevDy <= 2 && isNewShell; ++prevDy) {
                            for (int prevDx = -2; prevDx <= 2 && isNewShell; ++prevDx) {
                                int prevX = (windowPos.x + prevDx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
                                int prevY = (windowPos.y + prevDy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);
                                
                                if (static_cast<size_t>(prevX) < playerState.latestMap.size() &&
                                    static_cast<size_t>(prevY) < playerState.latestMap[0].size() &&
                                    playerState.latestMap[prevX][prevY] == '*') {
                                    isNewShell = false;
                                }
                            }
                        }
                    }
                    
                    if (isNewShell) {
                        // Search for tanks in a 2-cell radius to determine shell direction
                        bool foundTank = false;
                        for (int searchDy = -2; searchDy <= 2 && !foundTank; ++searchDy) {
                            for (int searchDx = -2; searchDx <= 2 && !foundTank; ++searchDx) {
                                int searchX = (windowPos.x + searchDx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
                                int searchY = (windowPos.y + searchDy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);
                                
                                char searchObj = map[searchX][searchY];
                                if (searchObj == '1' || searchObj == '2' || searchObj == '0' + playerId) {
                                    // Found a tank, calculate direction from tank to shell
                                    Position tankPos{searchX, searchY};
                                    Direction shellDir = DirectionUtils::directionFromTo(tankPos, windowPos);
                                    playerState.shellInfo[windowPos] = shellDir;
                                    foundTank = true;
                                }
                            }
                        }
                    } else {
                        // Shell was already here, keep its previous direction
                        if (playerState.shellInfo.find(windowPos) != playerState.shellInfo.end()) {
                            // Shell already has a direction, keep it
                            continue;
                        }
                    }
                } else if (obj == '0' + playerId) {
                    // For our tanks, check if position changed before updating direction
                    bool tankMoved = true;
                    for (const auto& [tankPos, _] : playerState.myTanksInfo) {
                        if (tankPos == windowPos) {
                            tankMoved = false;
                            break;
                        }
                    }
                    if (tankMoved) {
                        playerState.myTanksInfo.push_back({windowPos, dir});
                    }
                } else if (obj == '1' || obj == '2') {
                    // For enemy tanks, check if position changed before updating direction
                    bool tankMoved = true;
                    for (const auto& [tankPos, _] : playerState.enemyTanksInfo) {
                        if (tankPos == windowPos) {
                            tankMoved = false;
                            break;
                        }
                    }
                    if (tankMoved) {
                        playerState.enemyTanksInfo.push_back({windowPos, dir});
                    }
                }
            }
        }
    }
}

void ConcretePlayer::analyzeBoard(int tankId, SatelliteView& view) {
    auto newMap = readView(tankId, view);
    
    // First, update positions of existing shells using their known directions
    std::unordered_map<Position, Direction> updatedShells;
    for (const auto& [oldPos, shellDir] : playerState.shellInfo) {
        // Calculate expected position (2 cells in shell's direction)
        Position dirVector = DirectionUtils::dirToVector(shellDir);
        int expectedX = (oldPos.x + 2 * dirVector.x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
        int expectedY = (oldPos.y + 2 * dirVector.y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);
        
        // Only keep shell if it's exactly at the expected position
        if (newMap[expectedX][expectedY] == '*') {
            Position newPos{expectedX, expectedY};
            updatedShells[newPos] = shellDir;
        }
        // If shell isn't at expected position, it's gone (don't add to updatedShells)
    }
    
    // Update shell info with the ones we found
    playerState.shellInfo = std::move(updatedShells);
    playerState.myTanksInfo.clear();
    playerState.enemyTanksInfo.clear();
    
    // Now find any new shells on the map
    for (size_t y = 0; y < boardHeight; ++y) {
        for (size_t x = 0; x < boardWidth; ++x) {
            if (newMap[x][y] == '*') {
                Position shellPos{static_cast<int>(x), static_cast<int>(y)};
                
                // If this shell isn't in our updated positions, it's new
                if (playerState.shellInfo.find(shellPos) == playerState.shellInfo.end()) {
                    // Find closest tank to determine direction
                    Position closestTankPos;
                    int minDistance = std::numeric_limits<int>::max();
                    
                    // Search in a 2-cell radius for the closest tank
                    for (int searchDy = -2; searchDy <= 2; ++searchDy) {
                        for (int searchDx = -2; searchDx <= 2; ++searchDx) {
                            int searchX = (x + searchDx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
                            int searchY = (y + searchDy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);
                            
                            char searchObj = newMap[searchX][searchY];
                            if (searchObj == '1' || searchObj == '2' || searchObj == '0' + playerId) {
                                // Calculate distance (manhattan distance is fine for this)
                                int distance = std::abs(searchDx) + std::abs(searchDy);
                                if (distance < minDistance) {
                                    minDistance = distance;
                                    closestTankPos = Position{searchX, searchY};
                                }
                            }
                        }
                    }
                    
                    if (minDistance != std::numeric_limits<int>::max()) {
                        // Found a tank, calculate direction from tank to shell
                        Direction shellDir = DirectionUtils::directionFromTo(closestTankPos, shellPos);
                        playerState.shellInfo[shellPos] = shellDir;
                    }
                }
            } else if (newMap[x][y] == '1' || newMap[x][y] == '2') {
                // For tanks, analyze 1 cell in each direction
                Position pos{static_cast<int>(x), static_cast<int>(y)};
                analyzeWindowAroundObject(pos, 1, newMap);
            }
        }
    }
    
    playerState.latestMap = std::move(newMap);
}

void ConcretePlayer::handleTankTurnReordering(int tankId) {
    if(reorderTurns) {
        int turnMapSize = tankTurns.size();
        if (turnMapSize + 1 == aliveTanks){
            reorderTurns = false;
        }
        tankTurns[tankId] = turnMapSize;
    }
}