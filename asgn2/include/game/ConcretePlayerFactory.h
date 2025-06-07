#ifndef PLAYER_FACTORY_H
#define PLAYER_FACTORY_H

#include "../../common/PlayerFactory.h"
#include "ConcretePlayer.h"
#include "PlayerOne.h"
#include "PlayerTwo.h"
#include <stdexcept>

class ConcretePlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                 size_t max_steps, size_t num_shells) const override {
        // Validate inputs
        if (player_index != 1 && player_index != 2) {
            throw std::invalid_argument("Player index must be 1 or 2");
        }
        if (x == 0 || y == 0) {
            throw std::invalid_argument("Board dimensions must be positive");
        }
        if (max_steps == 0) {
            throw std::invalid_argument("Max steps must be positive");
        }
        if (num_shells == 0) {
            throw std::invalid_argument("Number of shells must be positive");
        }

        // Create player with validated inputs
        if (player_index == 1) {
            return std::make_unique<PlayerOne>(player_index, x, y, max_steps, num_shells);
        } else {
            return std::make_unique<PlayerTwo>(player_index, x, y, max_steps, num_shells);
        }
    }
};

#endif // PLAYER_FACTORY_H 