#ifndef TANK_ALGORITHM_FACTORY_H
#define TANK_ALGORITHM_FACTORY_H

#include "../../common/TankAlgorithmFactory.h"
#include "game/AlgorithmOne.h"
#include "game/AlgorithmTwo.h"
#include "game/Direction.h"
#include <stdexcept>

class ConcreteTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        // Validate inputs
        if (player_index != 1 && player_index != 2) {
            throw std::invalid_argument("Player index must be 1 or 2");
        }
        if (tank_index < 0) {
            throw std::invalid_argument("Tank index cannot be negative");
        }

        // Create algorithm with validated inputs
        std::unique_ptr<ConcreteTankAlgorithm> algo;
        if (player_index == 1) {
            algo = std::make_unique<AlgorithmOne>(player_index, tank_index);
        } else {
            algo = std::make_unique<AlgorithmTwo>(player_index, tank_index);
        }
        
        // Set initial direction based on player index (1 = LEFT, 2 = RIGHT)
        const Direction initialDirection = (player_index == 1) ? Direction::L : Direction::R;
        algo->setTankDir(initialDirection);
        
        return algo;
    }
};

#endif // TANK_ALGORITHM_FACTORY_H 