#include <dlfcn.h>
#include <filesystem>
#include <iostream>

#include "../common/GameManagerRegistration.h"
#include "GameManagerRegistrar.h"

GameManagerRegistration::GameManagerRegistration(GameManagerFactory factory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
}