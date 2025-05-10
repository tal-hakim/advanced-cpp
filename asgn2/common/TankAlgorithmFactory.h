//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_TANKALGORITHMFACTORY_H
#define ASGN2_TANKALGORITHMFACTORY_H
#include <memory>
#include "TankAlgorithm.h"
using namespace std;

class TankAlgorithmFactory {
public:
    virtual ~TankAlgorithmFactory() {}
    virtual unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index) const = 0;
};


#endif //ASGN2_TANKALGORITHMFACTORY_H
