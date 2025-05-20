#ifndef PLAYER_H
#define PLAYER_H

#include "../../common/Player.h"
#include "../../common/TankAlgorithm.h"
#include "../../common/SatelliteView.h"
#include "../../common/BattleInfo.h"
#include <memory>
#include <vector>
#include <unordered_map>

// Concrete Player implementation
class ConcretePlayer : public Player {
private:
    int playerId;
    size_t boardWidth;
    size_t boardHeight;
    size_t maxSteps;
    size_t numShells;
    std::unordered_map<int, std::unique_ptr<BattleInfo>> tankBattleInfo;

public:
    ConcretePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells),
          playerId(player_index),
          boardWidth(x),
          boardHeight(y),
          maxSteps(max_steps),
          numShells(num_shells) {}

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view, std::unique_ptr<BattleInfo> battle_info) override {
        int tankId = tank.getTankId();
        if (battle_info) {
            tankBattleInfo[tankId] = std::move(battle_info);
        }
        tankBattleInfo[tankId]->update(satellite_view);
        tank.updateBattleInfo(*tankBattleInfo[tankId]);
    }
};

#endif // PLAYER_H 