#include "algorithms/Evader.h"

Action Evader::getNextAction(const GameBoard& board,
                                      const Tank& myTank,
                                      const Tank& opponentTank) {
    if (myTank.canShoot()) {
        return Action::Shoot;  // still return fire if possible
    }

    // Dumb evasion: rotate or back up
    return Action::MoveBack;
}

