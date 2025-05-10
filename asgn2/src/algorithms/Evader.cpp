#include "algorithms/Evader.h"

ActionRequest Evader::getNextAction(const GameBoard& board,
                                    const Tank& myTank,
                                    const Tank& opponentTank) {

    Position tankPos = board.wrap(myTank.getPosition());
    Direction tankDir = myTank.getDirection();
    const int radius = 4;

    int closestThreatSteps = INT_MAX;
    const Shell* closestThreat = nullptr;

    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            Position checkPos = board.wrap({ tankPos.x + dx, tankPos.y + dy });

            const auto& objects = board.getObjectsAt(checkPos);
            for (const auto& obj : objects) {
                if (!obj) continue;

                auto* shell = dynamic_cast<Shell*>(obj.get());
                if (!shell) continue;

                int steps = stepsUntilShellHitsTank(*shell, myTank, board);
                if (steps != -1 && steps < closestThreatSteps) {
                    closestThreat = shell;
                    closestThreatSteps = steps;
                }
            }
        }
    }

    if (!closestThreat) {
//        return ActionRequest::DoNothing;
        if(stepsUntilShellHitsTank(Shell(myTank.getPosition(), myTank.getDirection(), myTank.getPlayerId()), opponentTank, board) >=0 && myTank.canShoot()){
            return ActionRequest::Shoot;
        }
        return checkOpponentAndAct(board, myTank, opponentTank);
    }


    // Defensive fire if directly aligned and not in cooldown
    if (Algorithm::areFacingEachOther(myTank, *closestThreat) &&
        myTank.canShoot()) {
        return ActionRequest::Shoot;
    }

    Direction shellDir = closestThreat->getDirection();
    Direction shellOpposite = DirectionUtils::getOppositeDirection(shellDir);

    if (shellDir != tankDir && shellOpposite != tankDir) {
        Position forwardPos = board.wrap(myTank.getNextPos());
        if (!board.isWall(forwardPos) && !board.isMine(forwardPos) && !board.isTank(forwardPos)) {
            return ActionRequest::MoveForward;
        }
    }

    for (int i = LEFT_ANGLE_1_4; i < 6; i++ ){

        int d = (static_cast<int>(myTank.getDirection()) + i + NUM_DIRECTIONS ) % NUM_DIRECTIONS ;
        Direction dir = static_cast<Direction>(d);
        Position tmpPos = board.wrap(myTank.getPosition() + DirectionUtils::dirToVector(dir));;
        if (!board.isWall(tmpPos) && !board.isMine(tmpPos)) {
            return rotateToward(myTank.getDirection(), dir);
        }
    }

    return ActionRequest::DoNothing; // fallback
}

ActionRequest Evader::checkOpponentAndAct(const GameBoard& board, const Tank& myTank, const Tank& opponentTank)
{
    // Check for opponent's tank in all 8 directions (one step or two steps)
    for (int d = 0; d < NUM_DIRECTIONS ; ++d) {
        Direction dir = static_cast<Direction>(d);
        Position firstStepPos = board.wrap(myTank.getPosition() + DirectionUtils::dirToVector(dir)); // First step

        // Check if the opponent is at either the first or second step
        if (firstStepPos == opponentTank.getPosition()) {
            // Opponent found one or two steps away, move or rotate accordingly
            Direction currentDir = myTank.getDirection();
            Direction desiredDir = dir;

            if (currentDir != desiredDir) {
                // Opponent is directly in front, so move forward if no obstacles
                if (!board.isWall(firstStepPos) && !board.isMine(firstStepPos)) {
                    return ActionRequest::MoveForward;
                }
            } else {
                // Rotate towards the opponent
                return rotateToward(currentDir, DirectionUtils::getOppositeDirection(desiredDir));
            }
        }

    }

    // If the opponent is not found in any direction, return a default action
    return ActionRequest::DoNothing;
}


