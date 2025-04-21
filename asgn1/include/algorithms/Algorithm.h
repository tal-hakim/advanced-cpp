#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "game/GameBoard.h"
#include "objects/Tank.h"
#include "game/Action.h"

class Algorithm {
public:
    virtual ~Algorithm() = default;

    virtual Action getNextAction(const GameBoard& board,
                                 const Tank& myTank,
                                 const Tank& opponentTank) = 0;
};

#endif // ALGORITHM_H
