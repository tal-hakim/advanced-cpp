//
// Created by talta on 21/06/2025.
//

#include "AlgorithmRegistrar.h"
AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() {
    return registrar;
}