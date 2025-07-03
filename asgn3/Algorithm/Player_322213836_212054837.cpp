//
// Created by talta on 21/06/2025.
//

#include "../common/PlayerRegistration.h"
#include "Player_322213836_212054837.h"
#include "ConcreteBattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "TankAlgorithm_322213836_212054837.h"
#include "../UserCommon/utils/MapUtils.h"
#include "../UserCommon/utils/definitions.h"
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

using namespace Algorithm_322213836_212054837;
REGISTER_PLAYER(Player_322213836_212054837);

namespace Algorithm_322213836_212054837 {
    using namespace MapUtils;  // Add MapUtils namespace at file scope

    Player_322213836_212054837::Player_322213836_212054837(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
            : maxSteps(maxSteps), numShells(numShells), aliveTanks(UNINITIALIZED), reorderTurns(false), 
              playerId(playerIndex), boardWidth(x), boardHeight(y) {
        // Initialize context with empty state
        playerState.latestMap.resize(x, std::vector<char>(y, EMPTY));
        playerState.initShells = numShells;

        roundCounter = 0;
    }

    void Player_322213836_212054837::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
        // player updates
        auto *tankPtr = dynamic_cast<TankAlgorithm_322213836_212054837 *>(&tank);
        int tankId = tankPtr->getTankId();
        if(tankId == 0){
            std::cout << "bla" << std::endl;
        }
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


    std::vector<std::vector<char>> Player_322213836_212054837::readView(int tankId, SatelliteView &view) {
        std::vector<std::vector<char>> map(boardWidth, std::vector<char>(boardHeight, EMPTY));
        int currAliveTanks = 0;

        // First pass: count tanks
        for (size_t i = 0; i < boardWidth; i++) {
            for (size_t j = 0; j < boardHeight; ++j) {
                char currObj = view.getObjectAt(i, j);
                if (currObj == '0' + playerId || currObj == CURR_TANK) {
                    currAliveTanks++;
                }
            }
        }

        // Initialize or resize myTanksInfo if needed
        if (this->aliveTanks == UNINITIALIZED) {
            playerState.myTanksInfo.resize(currAliveTanks, {Position{0, 0}, Direction::R});
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
        for (size_t i = 0; i < boardWidth; i++) {
            for (size_t j = 0; j < boardHeight; ++j) {
                char currObj = view.getObjectAt(i, j);
                if (currObj == CURR_TANK) {
                    playerState.myTanksInfo[tankId].first = Position{static_cast<int>(i), static_cast<int>(j)};
                    map[i][j] = '0' + playerId;
                } else {
                    map[i][j] = currObj;
                }
            }
        }

        if (currAliveTanks != this->aliveTanks) {
            this->aliveTanks = currAliveTanks;
            reorderTurns = true;
            tankTurns.clear();
        }
        return map;
    }

    void Player_322213836_212054837::analyzeWindowAroundObject(const Position &pos, int windowSize,
                                                   const std::vector<std::vector<char>> &map) {
        for (int dy = -windowSize; dy <= windowSize; ++dy) {
            for (int dx = -windowSize; dx <= windowSize; ++dx) {
                // Use MapUtils::wrap instead of manual wrapping
                Position windowPos = wrap(Position{pos.x + dx, pos.y + dy}, boardWidth, boardHeight);

                char obj = map[windowPos.x][windowPos.y];
                if (obj != EMPTY) {  // Only exclude empty space
                    Direction dir = DirectionUtils::directionFromTo(pos, windowPos);

                    if (obj == SHELL) {
                        // Check if this shell existed anywhere in a 5x5 area in the previous map
                        bool isNewShell = true;
                        if (!playerState.latestMap.empty()) {
                            for (int prevDy = -2; prevDy <= 2 && isNewShell; ++prevDy) {
                                for (int prevDx = -2; prevDx <= 2 && isNewShell; ++prevDx) {
                                    Position prevPos = wrap(Position{windowPos.x + prevDx, windowPos.y + prevDy},
                                                            boardWidth, boardHeight);

                                    if (static_cast<size_t>(prevPos.x) < playerState.latestMap.size() &&
                                        static_cast<size_t>(prevPos.y) < playerState.latestMap[0].size() &&
                                        playerState.latestMap[prevPos.x][prevPos.y] == SHELL) {
                                        isNewShell = false;
                                    }
                                }
                            }
                        }

                        if (isNewShell) {
                            playerState.shellInfo[windowPos] = dir;

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
                        for (const auto &[tankPos, _]: playerState.myTanksInfo) {
                            if (tankPos == windowPos) {
                                tankMoved = false;
                                break;
                            }
                        }
                        if (tankMoved) {
                            playerState.myTanksInfo.push_back({windowPos, dir});
                        }
                    } else if (isEnemyTank(obj, playerId)) {
                        // For enemy tanks, check if position changed before updating direction
                        bool tankMoved = true;
                        for (const auto &[tankPos, _]: playerState.enemyTanksInfo) {
                            if (tankPos == windowPos) {
                                tankMoved = false;
                                break;
                            }
                        }
                        if (tankMoved) {
                            // Use the direction calculated from movement
                            playerState.enemyTanksInfo.push_back({windowPos, dir});
                        }
                    }
                }

            }
        }
    }

    void Player_322213836_212054837::handleTankTurnReordering(int tankId) {
        if (reorderTurns) {
            int turnMapSize = tankTurns.size();
            if (turnMapSize + 1 == aliveTanks) {
                reorderTurns = false;
            }
            tankTurns[tankId] = turnMapSize;
        }
    }

    void Player_322213836_212054837::analyzeBoard(int tankId, SatelliteView &view) {
        /* TODO: make analyze board keep track of same id tanks, also try to find the killed tanks and remove them */
        auto newMap = readView(tankId, view);

        // Store old enemy positions before clearing
        std::vector<std::pair<Position, Direction>> oldEnemyTanks = std::move(playerState.enemyTanksInfo);

        // First, update positions of existing shells using their known directions
        std::unordered_map<Position, Direction> updatedShells;
        for (const auto &[oldPos, shellDir]: playerState.shellInfo) {
            // Calculate expected position (2 cells in shell's direction)
            Position dirVector = DirectionUtils::dirToVector(shellDir);
            int expectedX = (oldPos.x + 2 * dirVector.x + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
            int expectedY =
                    (oldPos.y + 2 * dirVector.y + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);

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

        // Now find any new shells on the map and analyze tanks
        for (size_t y = 0; y < boardHeight; ++y) {
            for (size_t x = 0; x < boardWidth; ++x) {
                if (newMap[x][y] == '*') {
                    Position shellPos{static_cast<int>(x), static_cast<int>(y)};

                    // If this shell isn't in our updated positions, it's new
                    if (playerState.shellInfo.find(shellPos) == playerState.shellInfo.end()) {
                        // Find closest tank to determine direction
                        Position closestTankPos = {UNINITIALIZED, UNINITIALIZED};
                        int minDistance = std::numeric_limits<int>::max();

                        // Search in a 2-cell radius for the closest tank
                        for (int searchDy = -2; searchDy <= 2; ++searchDy) {
                            for (int searchDx = -2; searchDx <= 2; ++searchDx) {
                                int searchX =
                                        (x + searchDx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
                                int searchY =
                                        (y + searchDy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);

                                char searchObj = newMap[searchX][searchY];
                                if (searchObj == PLAYER1_TANK || searchObj == PLAYER2_TANK || searchObj == '0' + playerId) {
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
                } else if (newMap[x][y] == PLAYER1_TANK || newMap[x][y] == PLAYER2_TANK) {
                    // For tanks, analyze 1 cell in each direction
                    Position pos{static_cast<int>(x), static_cast<int>(y)};
                    analyzeWindowAroundObject(pos, 1, newMap);
                }
            }
        }

        // Check each old enemy tank's position and its surroundings
        for (const auto &[oldPos, oldDir]: oldEnemyTanks) {
            bool foundEnemy = false;

            // Check in all 8 directions around the old position (including diagonals)
                for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                     int checkX = (oldPos.x + dx + static_cast<int>(boardWidth)) % static_cast<int>(boardWidth);
                    int checkY = (oldPos.y + dy + static_cast<int>(boardHeight)) % static_cast<int>(boardHeight);

                    char obj = newMap[checkX][checkY];
                    if (isEnemyTank(obj, playerId)) {
                        // Found an enemy tank in the surrounding area
                        Position newPos{checkX, checkY};
                        Direction newDir = (dx == 0 && dy == 0) ? oldDir : DirectionUtils::directionFromTo(oldPos,
                                                                                                           newPos);

                        // Check if this position is already in our enemyTanksInfo
                        bool alreadyTracked = false;
                        for (const auto &[currPos, _]: playerState.enemyTanksInfo) {
                            if (currPos == newPos) {
                                alreadyTracked = true;
                                break;
                            }
                        }

                        if (!alreadyTracked) {
                            playerState.enemyTanksInfo.push_back({newPos, newDir});
                        }
                        foundEnemy = true;
                        break;
                    }
                }
                if (foundEnemy) break;
            }

            // If we didn't find the tank in any surrounding position, mark it as dead
            if (!foundEnemy) {
                playerState.enemyTanksInfo.push_back({Position{-1, -1}, oldDir});
            }
        }

        playerState.latestMap = std::move(newMap);
    }


}