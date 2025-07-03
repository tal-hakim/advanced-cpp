#include "SmartHybridAlg.h"

using namespace SmartHybrid;

REGISTER_TANK_ALGORITHM(SmartHybridTank);
REGISTER_PLAYER(SmartHybridPlayer);

namespace SmartHybrid {

    SmartHybridTank::SmartHybridTank(int player_index, int tank_index)
            : player_index_(player_index), tank_index_(tank_index), danger(false), state(0) {}

    SmartHybridTank::~SmartHybridTank() {}

    ActionRequest SmartHybridTank::getAction() {
        if (danger) {
            // Alternate: Rotate, Move, Rotate, Move, etc.
            if (state % 2 == 0)
                return ActionRequest::RotateLeft90;
            else
                return ActionRequest::MoveForward;
        } else {
            // Cycle: MoveForward, MoveForward, DoNothing
            int pattern = state % 3;
            if (pattern == 0 || pattern == 1)
                return ActionRequest::MoveForward;
            else
                return ActionRequest::DoNothing;
        }
        ++state;
    }

    void SmartHybridTank::updateBattleInfo(::BattleInfo& info) {
        auto* hybridInfo = dynamic_cast<SmartHybridBattleInfo*>(&info);
        danger = (hybridInfo && hybridInfo->enemy_near);
        // Reset state to 0 when danger status changes
        static bool last_danger = false;
        if (danger != last_danger)
            state = 0;
        last_danger = danger;
        ++state;
    }

    SmartHybridPlayer::SmartHybridPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
            : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

    SmartHybridPlayer::~SmartHybridPlayer() {}

    void SmartHybridPlayer::updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) {
        SmartHybridBattleInfo info;
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

} // namespace SmartHybrid
