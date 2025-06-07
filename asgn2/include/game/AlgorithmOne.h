//
// Created by talta on 04/06/2025.
//

#ifndef ASGN2_ALGORITHMONE_H
#define ASGN2_ALGORITHMONE_H

#include "game/ConcreteTankAlgorithm.h"
class AlgorithmOne : public ConcreteTankAlgorithm{
public:
    AlgorithmOne(int player_index, int tank_index) : ConcreteTankAlgorithm(player_index, tank_index){};
    ActionRequest getAction() override;
};


#endif //ASGN2_ALGORITHMONE_H
