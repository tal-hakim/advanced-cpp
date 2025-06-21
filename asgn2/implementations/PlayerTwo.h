#ifndef PLAYER_TWO_H
#define PLAYER_TWO_H

#include "implementations/ConcretePlayer.h"
#include "implementations/ConcreteBattleInfo.h"
#include "implementations/ConcreteTankAlgorithm.h"
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

class PlayerTwo : public ConcretePlayer {
private:
    // Maps our tank ID to enemy tank globalIndex in enemyTanksInfo
    std::unordered_map<int, int> tankAssignments;
    // Maps our tank ID to its current BFS path
    std::unordered_map<int, std::vector<Position>> tankPaths;
    // Maps our tank ID to last known enemy position
    std::unordered_map<int, Position> lastEnemyPositions;

    // Helper methods
    void updateTankAssignments();
    bool shouldRecalculatePath(int tankId, const Position& enemyPos) const;
    bool isTankDead(int tankId);

public:
    PlayerTwo(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
            : ConcretePlayer(playerIndex, x, y, maxSteps, numShells) {}
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;

};

#endif // PLAYER_ONE_H

