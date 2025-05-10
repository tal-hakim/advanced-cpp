//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_PLAYERFACTORY_H
#define ASGN2_PLAYERFACTORY_H
#include <memory>
using namespace std;
#include "Player.h"

class PlayerFactory {
public:
    virtual ~PlayerFactory() {}
    virtual unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const = 0;
};

#endif //ASGN2_PLAYERFACTORY_H
