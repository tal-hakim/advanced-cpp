//
// Created by talta on 23/04/2025.
//
#include "algorithms/Algorithm.h"

int Algorithm::stepsUntilShellHitsTank(const Shell& shell, const Tank& tank, const GameBoard& board, int maxSteps) {
    Shell tmpShell = shell;
    Position tankPos = board.wrap(tank.getPosition());

    for (int step = 1; step <= maxSteps; ++step) {
        Position nextPos = tmpShell.getNextPos();            // Step 1: Predict
        Position wrappedPos = board.wrap(nextPos);           // Step 2: Wrap
        tmpShell.setPosition(wrappedPos);                    // Step 3: Apply

        if (board.isWall(wrappedPos)) {
            return -1; // Shell is destroyed on impact
        }

        if (wrappedPos == tankPos) {
            return step; // Shell hits tank in `step` moves
        }
    }

    return -1; // No hit
}

Direction Algorithm::getOppositeDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 4) % 8);
}

bool Algorithm::areFacingEachOther(const Tank& tank, const Shell& shell) {
    return tank.getDirection() == getOppositeDirection(shell.getDirection());
}


