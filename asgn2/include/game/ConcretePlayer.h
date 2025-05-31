#ifndef PLAYER_H
#define PLAYER_H

#include "../../common/Player.h"
#include "../../common/TankAlgorithm.h"
#include "../../common/SatelliteView.h"
#include "../../common/BattleInfo.h"
#include "game//Direction.h"
#include "game/DirectionUtils.h"
#include "game/Position.h"
#include "game/ConcreteBattleInfo.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>

// Concrete Player implementation
class ConcretePlayer : public Player {
private:
    // Base class parameters
    int playerId;
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;

    // State variables
    ConcreteBattleInfo::PlayerUpdates playerState;
    std::unordered_map<int, int> tankTurns;
    int aliveTanks;
    bool reorderTurns;
    size_t roundCounter = 0;
    std::map<int, int> remainingShells;

    void handleTankTurnReordering(int tankId);
    
    // Helper functions for board analysis
    void analyzeWindowAroundObject(const Position& pos, int windowSize, const std::vector<std::vector<char>>& map);

public:
    ConcretePlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
    void initializeTanksInfo(int numTanks) { playerState.myTanksInfo.resize(numTanks, {Position{0,0}, Direction::R}); }
    std::vector<std::vector<char>> readView(int tankId, SatelliteView &view);
    void analyzeBoard(int tankId, SatelliteView &view);
};

#endif // PLAYER_H 