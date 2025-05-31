#include <iostream>
#include "game/GameManager.h"
#include "game/ConcretePlayerFactory.h"
#include "game/ConcreteTankAlgorithmFactory.h"
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;  // Return error code for incorrect usage
    }

    try {
        ConcretePlayerFactory playerFactory;
        ConcreteTankAlgorithmFactory algorithmFactory;
        GameManager gameManager(playerFactory, algorithmFactory, argv[1]);

        if (!gameManager.readBoard(argv[1])) {
            return 1;  // Return error code for board reading failure
        }

        gameManager.runGame();
        return 0;
    } catch (const std::exception& e) {
        return 1;  // Return error code for any exception
    }
}

