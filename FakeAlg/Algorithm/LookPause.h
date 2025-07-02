#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

namespace LookAndPause {

    class LookAndPauseBattleInfo : public ::BattleInfo {
    public:
        virtual ~LookAndPauseBattleInfo() {}
    };

    class LookAndPauseTank : public ::TankAlgorithm {
        int player_index_, tank_index_;
        int turn = 0;
    public:
        LookAndPauseTank(int player_index, int tank_index)
                : player_index_(player_index), tank_index_(tank_index) {}

        virtual ~LookAndPauseTank() {}
        virtual ActionRequest getAction() override {
            if (turn % 2 == 0) {
                turn++;
                return ActionRequest::DoNothing;
            } else {
                turn++;
                return ActionRequest::RotateRight45;
            }
        }
        virtual void updateBattleInfo(::BattleInfo&) override {}
    };

    class LookAndPausePlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
    public:
        LookAndPausePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
                : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

        virtual ~LookAndPausePlayer() {}
        virtual void updateTankWithBattleInfo(::TankAlgorithm&, ::SatelliteView&) override {}
    };

}
