#ifndef ALGORITHMEVADER_H
#define ALGORITHMEVADER_H

#include "Algorithm.h"

class Evader : public Algorithm {
public:
    Action getNextAction(const GameBoard& board,
                         const Tank& myTank,
                         const Tank& opponentTank) override;
};



#endif // ALGORITHMEVADER_H
