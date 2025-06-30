//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_TANKALGORITHMREGISTRATION_H
#define ASGN3_TANKALGORITHMREGISTRATION_H

#include "TankAlgorithm.h"

struct TankAlgorithmRegistration {
    TankAlgorithmRegistration(TankAlgorithmFactory);
};

#define REGISTER_TANK_ALGORITHM(class_name) \
TankAlgorithmRegistration register_me_##class_name \
	( [](int player_index, int tank_index) { \
	    return std::make_unique<class_name>(player_index, tank_index); } \
	);
#endif //ASGN3_TANKALGORITHMREGISTRATION_H
