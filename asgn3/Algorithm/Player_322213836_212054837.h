//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_PLAYER_322213836_212054837_H
#define ASGN3_PLAYER_322213836_212054837_H

#include "../common/Player.h"
#include "../common/TankAlgorithm.h"
#include "../common/SatelliteView.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "ConcreteBattleInfo.h"
#include "../UserCommon/utils/Position.h"
#include <vector>
using namespace UserCommon_322213836_212054837;
namespace Algorithm_322213836_212054837 {

using std::unique_ptr;
using std::vector;

class Player_322213836_212054837: public Player {
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

        size_t boardWidth;
        size_t boardHeight;
        std::unordered_map<int, int> tankTurns;
        void handleTankTurnReordering(int tankId);
        size_t roundCounter = 0;
        ConcreteBattleInfo::PlayerUpdates playerState;
        std::map<int, int> remainingShells;


    public:
        Player_322213836_212054837(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);
        ~Player_322213836_212054837() = default;
//        void initializeTanksInfo(int numTanks) { playerState.myTanksInfo.resize(numTanks, {Position{0,0}, Direction::R}); }
        void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
        std::vector<std::vector<char>> readView(SatelliteView &view);
        void analyzeBoard(SatelliteView &view);
    };

}

#endif //ASGN3_PLAYER_322213836_212054837_H
