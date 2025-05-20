#ifndef PLAYER_FACTORY_H
#define PLAYER_FACTORY_H

#include "../../common/PlayerFactory.h"
#include "ConcretePlayer.h"

class ConcretePlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                 size_t max_steps, size_t num_shells) const override {
        return std::make_unique<ConcretePlayer>(player_index, x, y, max_steps, num_shells);
    }
};

#endif // PLAYER_FACTORY_H 