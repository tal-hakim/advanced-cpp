#include <iostream>
#include "game/GameManager.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];

    GameManager gameManager(inputFile);

    gameManager.runGame();  // or whatever your method is to start the game

    return 0;
}