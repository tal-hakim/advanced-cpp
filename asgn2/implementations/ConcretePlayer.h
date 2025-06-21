#ifndef PLAYER_H
#define PLAYER_H

#include "common/Player.h"
#include "common/TankAlgorithm.h"
#include "common/SatelliteView.h"
#include "common/BattleInfo.h"
#include "utils/Direction.h"
#include "utils/DirectionUtils.h"
#include "utils/Position.h"
#include "ConcreteBattleInfo.h"
#include "utils/MapUtils.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>

// Concrete Player implementation
class ConcretePlayer : public Player {
private:
    // Base class parameters

    size_t maxSteps;
    size_t numShells;

    // State variables
    int aliveTanks;
    bool reorderTurns;


    // Helper functions for board analysis
protected:
    int playerId;

    void analyzeWindowAroundObject(const Position& pos, int windowSize, const std::vector<std::vector<char>>& map);
    ConcreteBattleInfo::PlayerUpdates playerState;
    size_t boardWidth;
    size_t boardHeight;
    std::unordered_map<int, int> tankTurns;
    void handleTankTurnReordering(int tankId);
    size_t roundCounter = 0;
    std::map<int, int> remainingShells;



 

public:
    ConcretePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    void initializeTanksInfo(int numTanks) { playerState.myTanksInfo.resize(numTanks, {Position{0,0}, Direction::R}); }
    std::vector<std::vector<char>> readView(int tankId, SatelliteView &view);
    void analyzeBoard(int tankId, SatelliteView& view);
};

#endif // PLAYER_H 