#include "algorithms/Chaser.h"
#include <cmath>

Action Chaser::getNextAction(const GameBoard& board,
                             const Tank& myTank,
                             const Tank& opponentTank) {
    static int stepCounter = 0;
    ++stepCounter;

    if (myTank.canShoot()) {
        return Action::Shoot;
    }

    // Move forward every 3rd step, rotate otherwise
    if (stepCounter % 3 == 0) {
        return Action::MoveFwd;
    } else {
        return Action::RotateRight_1_8;
    }
}


