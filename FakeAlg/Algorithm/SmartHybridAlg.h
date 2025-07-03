#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

namespace SmartHybrid {

    class SmartHybridBattleInfo : public ::BattleInfo {
    public:
        bool enemy_near = false;
        SmartHybridBattleInfo() = default;
        virtual ~SmartHybridBattleInfo() {}
    };

    class SmartHybridTank : public ::TankAlgorithm {
        int player_index_, tank_index_;
        bool danger = false;
        int state = 0; // Used for cycling through behaviors
    public:
        SmartHybridTank(int player_index, int tank_index);
        virtual ~SmartHybridTank();
        virtual ActionRequest getAction() override;
        virtual void updateBattleInfo(::BattleInfo& info) override;
    };

    class SmartHybridPlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
    public:
        SmartHybridPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        virtual ~SmartHybridPlayer();
        virtual void updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) override;
    };

} // namespace SmartHybrid
