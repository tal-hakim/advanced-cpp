#ifndef CONCRETE_TANK_ALGORITHM_H
#define CONCRETE_TANK_ALGORITHM_H

#include "../../common/TankAlgorithm.h"
#include "../../common/ActionRequest.h"
#include "../../common/BattleInfo.h"
#include "game/ConcreteBattleInfo.h"
#include "game/GameBoard.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "algorithms/Algorithm.h"
#include <vector>
#include <unordered_set>
#include <string>
#include "Direction.h"

class ConcreteTankAlgorithm : public TankAlgorithm {
private:
    ConcreteBattleInfo::PlayerUpdates playerState;
    ConcreteBattleInfo::TankUpdates tankState;
    int playerId;
    int tankId;
    size_t numTanks;  // Track total number of tanks in the game
    std::vector<Position> bfsPath;  // Store the current BFS path
    Position lastEnemyPos;  // Store last known enemy position for path recalculation
    int lastShotRound;  // Track the last round we shot

public:
    ConcreteTankAlgorithm(int player_index, int tank_index)
            : playerId(player_index), tankId(tank_index),
              numTanks(0), lastEnemyPos(Position{0,0}), lastShotRound(-4) {
        playerState.turnId = tank_index;  // Set turnId to be the tank's index
        tankState.roundCounter = UNINITIALIZED;
    }
    virtual ~ConcreteTankAlgorithm() = default;

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    int getTankId() const {return tankId;}
    void setTankDir(Direction dir) { tankState.tankDir = dir; }
    void setTankId(int id) { playerState.turnId = id; }  // Set the turnId to be the tank's index
};

#endif // CONCRETE_TANK_ALGORITHM_H 