#include "algorithms/Evader.h"

Action Evader::getNextAction(const GameBoard& board,
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

    if (!closestThreat)
        return Action::None;

    // Defensive fire if directly aligned and not in cooldown
    if (Algorithm::areFacingEachOther(myTank, *closestThreat) &&
        myTank.canShoot()) {
        return Action::Shoot;
    }

    Direction shellDir = closestThreat->getDirection();
    Direction shellOpposite = Algorithm::getOppositeDirection(shellDir);

    if (shellDir != tankDir && shellOpposite != tankDir) {
        Position forwardPos = board.wrap(myTank.getNextPos());
        if (!board.isWall(forwardPos) && !board.isMine(forwardPos) && !board.isTank(forwardPos)) {
            return Action::MoveFwd;
        }
        for (int i = LEFT_ANGLE_1_4; i <= RIGHT_ANGLE_1_4; i++ ){

        }
    }

    return Action::None; // fallback
}

