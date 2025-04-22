#include <iostream>
#include "game/GameManager.h"

int main() {
    GameManager manager = GameManager(R"(/home/tal/dev/advanced-cpp/asgn1/input/input1.txt)");
    manager.runGame();
    return 0;
}
