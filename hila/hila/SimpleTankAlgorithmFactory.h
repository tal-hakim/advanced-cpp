#ifndef SIMPLE_TANK_ALGORITHM_FACTORY_H
#define SIMPLE_TANK_ALGORITHM_FACTORY_H   
#include "common/TankAlgorithmFactory.h"
#include "../configuration/Config.h"
#include "BFSTankAlgorithm.h"
#include "RotatingTankAlgorithm.h"
#include <iostream>

class SimpleTankAlgorithmFactory : public TankAlgorithmFactory {
    private:
        static constexpr int DEFAULT_BATTLE_INFO_REQUEST_PERIOD = 3; // Default period for requesting battle info, can be configured in congig.txt
        int battle_info_request_period;
    
    public:
        virtual ~SimpleTankAlgorithmFactory() = default;
        SimpleTankAlgorithmFactory();
        virtual unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};

#endif