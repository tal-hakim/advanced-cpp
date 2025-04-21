#ifndef ALGORITHMCHASER_H
#define ALGORITHMCHASER_H

#include "Algorithm.h"

class Chaser : public Algorithm {
public:
    Action getNextAction(const GameBoard& board,
                         const Tank& myTank,
                         const Tank& opponentTank) override;
};

#endif // ALGORITHMCHASER_H
