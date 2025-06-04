#ifndef ANALYTICAL_PLAYER_H
#define ANALYTICAL_PLAYER_H

#include "../../common/Player.h"
#include "../../common/TankAlgorithm.h"
#include "../../common/SatelliteView.h"
#include "../../common/BattleInfo.h"
#include "game/Direction.h"
#include "game/DirectionUtils.h"
#include "game/Position.h"
#include "game/ConcreteBattleInfo.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

// A different implementation of Player that uses analytical approaches
class AnalyticalPlayer : public Player {
private:
    // Base class parameters
    int playerId;
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;

    // State variables
    struct GameState {
        std::vector<std::pair<Position, Direction>> myTanksInfo;
        std::vector<std::pair<Position, Direction>> enemyTanksInfo;
        std::vector<std::vector<char>> latestMap;
        std::unordered_map<Position, Direction> shellInfo;
        std::unordered_map<Position, int> dangerMap;  // Maps positions to danger level
        std::unordered_map<Position, int> valueMap;   // Maps positions to strategic value
        size_t turnId;
        int initShells;
    };
    GameState gameState;
    
    // Analysis state
    struct TankState {
        int tankId;
        Position pos;
        Direction dir;
        int remainingShells;
        int lastShotRound;
        std::vector<Position> safeZones;  // Positions this tank can safely move to
        std::vector<Position> attackZones;  // Positions from which this tank can attack
    };
    std::unordered_map<int, TankState> tankStates;
    
    // Strategic analysis
    struct StrategicInfo {
        std::set<Position> highValuePositions;  // Positions worth controlling
        std::set<Position> dangerZones;         // Areas to avoid
        std::map<Position, std::vector<Position>> attackPaths;  // Paths to attack positions
    };
    StrategicInfo strategy;

    // Helper methods
    void analyzeDangerZones();
    void analyzeStrategicPositions();
    void updateTankStates();
    void calculateAttackPaths();
    bool isPositionSafe(const Position& pos, int tankId) const;
    int calculatePositionValue(const Position& pos) const;
    std::vector<Position> findSafePath(const Position& start, const Position& goal, int tankId) const;
    void updateDangerMap();
    void predictShellMovements();
    std::vector<Position> getLineOfSight(const Position& start, Direction dir) const;

public:
    AnalyticalPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
    void initializeTanksInfo(int numTanks);
};

#endif // ANALYTICAL_PLAYER_H 