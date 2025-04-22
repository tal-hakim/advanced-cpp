#include "algorithms/Chaser.h"
#include <cmath>

Action Chaser::getNextAction(const GameBoard& board,
                                      const Tank& myTank,
                                      const Tank& opponentTank) {
    // Naive chase: if aligned, shoot. Else, move forward.
    Position myPos = myTank.getPosition();
    Position enemyPos = opponentTank.getPosition();
    std::cout << myPos << std::endl;
    if (myTank.canShoot()) {
        // Just try to shoot constantly in naive mode
        return Action::Shoot;
    }

    return Action::MoveFwd;
}

