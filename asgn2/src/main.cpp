#include <iostream>
#include "game/GameManager.h"
#include "implementations/ConcretePlayerFactory.h"
#include "implementations/ConcreteTankAlgorithmFactory.h"
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;  // Return error code for incorrect usage
    }

    ConcretePlayerFactory playerFactory;
    ConcreteTankAlgorithmFactory algorithmFactory;
    GameManager gameManager(playerFactory, algorithmFactory, argv[1]);

    gameManager.readBoard(argv[1]);

    gameManager.runGame();
    return 0;
}

