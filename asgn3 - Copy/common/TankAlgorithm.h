//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_TANKALGORITHM_H
#define ASGN3_TANKALGORITHM_H
#include <memory>
#include <functional>

#include "ActionRequest.h"
#include "BattleInfo.h"


class TankAlgorithm {
public:
    virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};

using TankAlgorithmFactory =
        std::function<std::unique_ptr<TankAlgorithm>(int player_index, int tank_index)>;
#endif //ASGN3_TANKALGORITHM_H
