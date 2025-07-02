//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_GAMEMANAGERREGISTRATION_H
#define ASGN3_GAMEMANAGERREGISTRATION_H

#include <functional>
#include "AbstractGameManager.h"

struct GameManagerRegistration {
    GameManagerRegistration(GameManagerFactory);
};

#define REGISTER_GAME_MANAGER(class_name) \
GameManagerRegistration register_me_##class_name \
        ( [] (bool verbose) { return std::make_unique<class_name>(verbose); } );


#endif //ASGN3_GAMEMANAGERREGISTRATION_H
