#ifndef CONCRETE_TANK_ALGORITHM_H
#define CONCRETE_TANK_ALGORITHM_H

#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "common/BattleInfo.h"
#include "ConcreteBattleInfo.h"
#include "game/GameBoard.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "algorithms/Algorithm.h"
#include "definitions.h"
#include <vector>
#include <unordered_set>
#include <string>
#include <optional>
#include "Direction.h"
#include "definitions.h"
#include "utils/MapUtils.h"

class ConcreteTankAlgorithm : public TankAlgorithm {
private:
    int playerId;
    size_t numTanks;  // Track total number of tanks in the game
    size_t mapWidth;  // Map width, updated with battle info
    size_t mapHeight; // Map height, updated with battle info
    Position lastEnemyPos;  // Store last known enemy position for path recalculation
    int lastShotRound;  // Track the last round we shot
protected:
    ConcreteBattleInfo::PlayerUpdates playerState;
    int tankId;
    ConcreteBattleInfo::TankUpdates tankState;
    std::vector<Position> bfsPath;  // Store the current BFS path

    void updateShellPositions();  // Helper function to update shell positions


public:
    // ====== ThreatInfo Struct ======
    struct ThreatInfo {
        bool exists = false;
        Position pos;
        Direction dir;
        int steps = INT_MAX;
    };

    ConcreteTankAlgorithm(int player_index, int tank_index)
            : playerId(player_index), numTanks(0),
              mapWidth(static_cast<size_t>(UNINITIALIZED)), mapHeight(static_cast<size_t>(UNINITIALIZED)), lastEnemyPos(Position{0,0}),
              lastShotRound(-4), tankId(tank_index) {
        playerState.turnId = tank_index;  // Set turnId to be the tank's globalIndex
        tankState.roundCounter = UNINITIALIZED;
        tankState.remainingShells = UNINITIALIZED;
    }
    virtual ~ConcreteTankAlgorithm() = default;

    void updateBattleInfo(BattleInfo& info) override;
    int getTankId() const {return tankId;}
    void setTankDir(Direction dir) { tankState.tankDir = dir; }
    void setTankId(int id) { playerState.turnId = id; }  // Set the turnId to be the tank's globalIndex
    bool canShootEnemy(Direction dir, const Position &myPos, const Position &enemyPos) const;

    std::vector<Direction> getSafeMoves(const Position &myPos) const;

    std::vector<Direction> getSafeRotations(const Position &myPos) const;

    bool shouldRecalculatePath(const Position &enemyPos) const;

    ActionRequest shoot();

    bool canShoot() const;

    virtual ActionRequest chaseEnemy(Position myPos, Direction myDir);

    ActionRequest evadeThreat(const ThreatInfo &threat, Position myPos, Direction myDir);

    ThreatInfo detectThreat(const Position &myPos) const;

    bool shouldGetBattleInfo() const;

    std::vector<Direction> getAllDirections() const;

    std::optional<Position> getNearestEnemy(const Position& myPos);

    bool pathBlocked(const std::vector<Position> &path) const;

    ActionRequest rotateToward(Direction current, Direction target);

    bool canMoveForward(Position myPos, Direction myDir);
};

#endif // CONCRETE_TANK_ALGORITHM_H 