//
// Created by talta on 02/07/2025.
//

#pragma once
#include "../common/ActionRequest.h"
#include "../common/TankAlgorithm.h"
#include "../common/BattleInfo.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithmRegistration.h"
#include "../common/PlayerRegistration.h"

namespace SpinShooter {

    class SpinShooterBattleInfo : public ::BattleInfo {
    public:
        virtual ~SpinShooterBattleInfo() {}
    };

    class SpinShooterTank : public ::TankAlgorithm {
        int player_index_, tank_index_;
        int step = 0;
    public:
        SpinShooterTank(int player_index, int tank_index)
                : player_index_(player_index), tank_index_(tank_index) {}

        virtual ~SpinShooterTank() {}
        virtual ActionRequest getAction() override {
            // Alternate between rotating and shooting
            step++;
            if (step % 3 == 0)
                return ActionRequest::Shoot;
            else if (step % 3 == 1)
                return ActionRequest::RotateRight90;
            else
                return ActionRequest::RotateLeft90;
        }
        virtual void updateBattleInfo(::BattleInfo&) override {}
    };

    class SpinShooterPlayer : public ::Player {
        int player_index_;
        size_t x_, y_, max_steps_, num_shells_;
    public:
        SpinShooterPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
                : player_index_(player_index), x_(x), y_(y), max_steps_(max_steps), num_shells_(num_shells) {}

        virtual ~SpinShooterPlayer() {}
        virtual void updateTankWithBattleInfo(::TankAlgorithm&, ::SatelliteView&) override {}
    };

}

