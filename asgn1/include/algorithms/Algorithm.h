#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "game/GameBoard.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "game/Action.h"

class Algorithm {
public:
    virtual ~Algorithm() = default;

    virtual Action getNextAction(const GameBoard& board,
                                 const Tank& myTank,
                                 const Tank& opponentTank) = 0;

protected:
    int stepsUntilShellHitsTank(const Shell& shell, const Tank& tank, const GameBoard& board, int maxSteps = 10);

    static bool areFacingEachOther(const Tank &tank, const Shell &shell);

    static Action rotateToward(Direction current, Direction target);
};

#endif // ALGORITHM_H
