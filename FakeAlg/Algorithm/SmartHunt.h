#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

#include <vector>
#include <map>

namespace SmartHunt {

    class SmartHuntBattleInfo : public ::BattleInfo {
    public:
        std::vector<std::pair<size_t, size_t>> enemy_positions;

        SmartHuntBattleInfo() = default;
        virtual ~SmartHuntBattleInfo() {}
    };

    class SmartHuntTank : public ::TankAlgorithm {
        int player_index_, tank_index_;
        int turn = 0;
        bool seen_enemy = false;
    public:
        SmartHuntTank(int player_index, int tank_index);

        virtual ~SmartHuntTank();
        virtual ActionRequest getAction() override;
        virtual void updateBattleInfo(::BattleInfo& info) override;
    };

    class SmartHuntPlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
        std::map<int, std::vector<ActionRequest>> tank_action_log;
    public:
        SmartHuntPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

        virtual ~SmartHuntPlayer();
        virtual void updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) override;
    };

} // namespace SmartHunt

