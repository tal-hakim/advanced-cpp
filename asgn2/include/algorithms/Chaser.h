#ifndef ALGORITHMCHASER_H
#define ALGORITHMCHASER_H
#include "algorithms/Chaser.h"
#include "Algorithm.h"
#include <vector>

class Chaser : public Algorithm {
private:
    std::vector<Position> bfsPath;
    Position lastEnemyPos;
    int lastStep = -1;

public:
    ActionRequest getNextAction(const GameBoard& board,
                                const Tank& myTank,
                                const Tank& opponentTank) override;
};

#endif // ALGORITHMCHASER_H
