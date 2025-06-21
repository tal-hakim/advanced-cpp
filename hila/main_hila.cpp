#include <iostream>
#include "game/GameManager.h"
#include "implementations/ConcretePlayerFactory.h"
#include "implementations/ConcreteTankAlgorithmFactory.h"
#include "hila/SimplePlayerFactory.h"
#include "hila/SimpleTankAlgorithmFactory.h"
#include <string>
#include <memory>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;  // Return error code for incorrect usage
    }

    SimplePlayerFactory playerFactory;
    SimpleTankAlgorithmFactory algorithmFactory;
    GameManager gameManager(playerFactory, algorithmFactory, argv[1]);

    gameManager.readBoard(argv[1]);

    gameManager.runGame();
    return 0;
}

