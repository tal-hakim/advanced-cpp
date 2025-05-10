#ifndef ALGORITHMEVADER_H
#define ALGORITHMEVADER_H

#include "Algorithm.h"

class Evader : public Algorithm {
public:
    ActionRequest getNextAction(const GameBoard& board,
                                const Tank& myTank,
                                const Tank& opponentTank) override;

    ActionRequest checkOpponentAndAct(const GameBoard &board, const Tank &myTank, const Tank &opponentTank);
};



#endif // ALGORITHMEVADER_H
