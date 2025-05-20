#ifndef TANK_ALGORITHM_FACTORY_H
#define TANK_ALGORITHM_FACTORY_H

#include "../../common/TankAlgorithmFactory.h"
#include "algorithms/Chaser.h"
#include "algorithms/Evader.h"

class ConcreteTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        // For now, we'll use Chaser for player 1 and Evader for player 2
        // You can modify this to create different algorithms based on player_index and tank_index
        if (player_index == 1) {
            return std::make_unique<Chaser>();
        } else {
            return std::make_unique<Evader>();
        }
    }
};

#endif // TANK_ALGORITHM_FACTORY_H 