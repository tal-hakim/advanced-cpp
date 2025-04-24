#include <iostream>
#include "game/GameManager.h"

int main() {
    GameManager manager = GameManager("../input/input1.txt");
    manager.runGame();
    return 0;
}
