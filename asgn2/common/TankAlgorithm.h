//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_TANKALGORITHM_H
#define ASGN2_TANKALGORITHM_H

#include "ActionRequest.h"
#include "BattleInfo.h"

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};


#endif //ASGN2_TANKALGORITHM_H
