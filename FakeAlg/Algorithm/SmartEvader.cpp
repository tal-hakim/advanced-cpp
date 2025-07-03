//
// Created by talta on 03/07/2025.
//

#include "SmartEvader.h"

using namespace SmartEvader;

// Registration macros MUST be at global scope, not in namespace!
REGISTER_TANK_ALGORITHM(SmartEvaderTank);
REGISTER_PLAYER(SmartEvaderPlayer);

namespace SmartEvader {

    SmartEvaderTank::SmartEvaderTank(int player_index, int tank_index)
            : player_index_(player_index), tank_index_(tank_index), danger(false) {}

    SmartEvaderTank::~SmartEvaderTank() {}

    ActionRequest SmartEvaderTank::getAction() {
        // If there's danger, rotate to "evade", else move forward.
        if (danger)
            return ActionRequest::RotateRight90;
        else
            return ActionRequest::MoveForward;
    }

    void SmartEvaderTank::updateBattleInfo(::BattleInfo& info) {
        auto* evaderInfo = dynamic_cast<SmartEvaderBattleInfo*>(&info);
        danger = evaderInfo && evaderInfo->enemy_near;
    }

    SmartEvaderPlayer::SmartEvaderPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
            : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

    SmartEvaderPlayer::~SmartEvaderPlayer() {}

    void SmartEvaderPlayer::updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) {
        SmartEvaderBattleInfo info;
        // Scan a 5x5 grid for enemy presence
        for (size_t i = 0; i < 5; ++i) {
            for (size_t j = 0; j < 5; ++j) {
                if (satellite_view.getObjectAt(i, j) == 'E') {
                    info.enemy_near = true;
                    break;
                }
            }
            if (info.enemy_near)
                break;
        }
        tank.updateBattleInfo(info);
    }

} // namespace SmartEvader
