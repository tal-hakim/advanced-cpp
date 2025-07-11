//
// Created by talta on 23/04/2025.
//
#include "algorithms/Algorithm.h"
#include "utils/DirectionUtils.h"

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


bool Algorithm::areFacingEachOther(const Tank& tank, const Shell& shell) {
    return tank.getDirection() == DirectionUtils::getOppositeDirection(shell.getDirection());
}


ActionRequest Algorithm::rotateToward(Direction current, Direction target) {
    int rawCurrent = static_cast<int>(current);
    int rawTarget = static_cast<int>(target);
    int diff = (rawTarget - rawCurrent + NUM_DIRECTIONS ) % NUM_DIRECTIONS ;

    if (diff == 0) return ActionRequest::DoNothing;
    if (diff == 1 || diff == 7) return (diff == 1) ? ActionRequest::RotateRight45 : ActionRequest::RotateLeft45;
//        if (diff == 2 || diff == 6) return (diff == 2) ? ActionRequest::RotateRight90 : ActionRequest::RotateLeft90;
    return (diff <= 4) ? ActionRequest::RotateRight90 : ActionRequest::RotateLeft90;
}





