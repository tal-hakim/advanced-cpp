#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

// ---- BattleInfo ----
namespace AlwaysForward {

    class BattleInfo : public ::BattleInfo {
    public:
        virtual ~BattleInfo() {}
    };

    class ForwardAlgorithm : public ::TankAlgorithm {
        int player_index_, tank_index_;
    public:
        ForwardAlgorithm(int player_index, int tank_index)
                : player_index_(player_index), tank_index_(tank_index) {}

        virtual ~ForwardAlgorithm() {}
        virtual ActionRequest getAction() override {
            return ActionRequest::MoveForward;
        }
        virtual void updateBattleInfo(::BattleInfo&) override {}
    };

    class ForwardPlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
    public:
        ForwardPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
                : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

        virtual ~ForwardPlayer() {}
        virtual void updateTankWithBattleInfo(
                ::TankAlgorithm&, ::SatelliteView&) override {}
    };

}