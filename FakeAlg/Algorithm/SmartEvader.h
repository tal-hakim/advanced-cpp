#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

namespace SmartEvader {

    class SmartEvaderBattleInfo : public ::BattleInfo {
    public:
        bool enemy_near = false;
        SmartEvaderBattleInfo() = default;
        virtual ~SmartEvaderBattleInfo() {}
    };

    class SmartEvaderTank : public ::TankAlgorithm {
        int player_index_, tank_index_;
        bool danger = false;
    public:
        SmartEvaderTank(int player_index, int tank_index);
        virtual ~SmartEvaderTank();
        virtual ActionRequest getAction() override;
        virtual void updateBattleInfo(::BattleInfo& info) override;
    };

    class SmartEvaderPlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
    public:
        SmartEvaderPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        virtual ~SmartEvaderPlayer();
        virtual void updateTankWithBattleInfo(::TankAlgorithm& tank, ::SatelliteView& satellite_view) override;
    };

} // namespace SmartEvader

