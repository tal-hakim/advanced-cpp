#include "SmartHunt.h"
#include <iostream>
using namespace SmartHunt;
REGISTER_TANK_ALGORITHM(SmartHuntTank);
REGISTER_PLAYER(SmartHuntPlayer);

namespace SmartHunt {

    SmartHuntTank::SmartHuntTank(int player_index, int tank_index)
            : player_index_(player_index), tank_index_(tank_index) {}

    SmartHuntTank::~SmartHuntTank() {}

    ActionRequest SmartHuntTank::getAction() {
        // Pretend to hunt: alternate between searching and shooting.
        turn++;
        if (seen_enemy) {
            return (turn % 2 == 0) ? ActionRequest::Shoot : ActionRequest::MoveForward;
        } else {
            if (turn % 3 == 0)
                return ActionRequest::RotateRight90;
            else
                return ActionRequest::MoveForward;
        }
    }

    void SmartHuntTank::updateBattleInfo(::BattleInfo& info) {
        // Try to dynamic_cast to our BattleInfo to "see" enemies
        SmartHuntBattleInfo* smbi = dynamic_cast<SmartHuntBattleInfo*>(&info);
        if (smbi && !smbi->enemy_positions.empty())
            seen_enemy = true;
        else
            seen_enemy = false;
    }

// SmartHuntPlayer Implementation

    SmartHuntPlayer::SmartHuntPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
            : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

    SmartHuntPlayer::~SmartHuntPlayer() {}

    void SmartHuntPlayer::updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) {
        // Example: If the tank can "see" an enemy (object 'E' on the board), send info
        SmartHuntBattleInfo info;
        for (size_t i = 0; i < 5; ++i) {
            for (size_t j = 0; j < 5; ++j) {
                if (satellite_view.getObjectAt(i, j) == 'E') {
                    info.enemy_positions.emplace_back(i, j);
                }
            }
        }
        tank.updateBattleInfo(info);

        // For demonstration, you could log the action (not used by framework, but realistic)
        // tank_action_log[tank_index].push_back(??); // If you had a tank index
    }

// Registration macros

} // namespace SmartHunt

