//
// Created by talta on 21/06/2025.
//

#include "../common/PlayerRegistration.h"
#include "AlgorithmRegistrar.h"

PlayerRegistration::PlayerRegistration(PlayerFactory factory) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    registrar.addPlayerFactoryToLastEntry(std::move(factory));
}