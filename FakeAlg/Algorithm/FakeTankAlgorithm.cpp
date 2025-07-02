//
// Created by talta on 21/06/2025.
//

#include "../common/TankAlgorithmRegistration.h"
#include "FakeTankAlgorithm.h"

using namespace FakeAlgorithm;
REGISTER_TANK_ALGORITHM(FakeTankAlgorithm);
namespace FakeAlgorithm {
    using namespace MapUtils;

    ActionRequest FakeTankAlgorithm::getAction() {
        return ActionRequest::Shoot;
    }

}