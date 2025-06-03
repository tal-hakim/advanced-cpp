#include <iostream>
#include "game/GameManager.h"
#include "game/ConcretePlayerFactory.h"
#include "game/ConcreteTankAlgorithmFactory.h"
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

