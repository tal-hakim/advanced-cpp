#include <iostream>
#include "game/GameManager.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    GameManager game(MyPlayerFactory(), MyTankAlgorithmFactory(), inputFile);
    game.readBoard(inputFile);
    game.runGame();
    return 0;
}

