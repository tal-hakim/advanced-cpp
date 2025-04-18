#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


#include "GameBoard.h"
//#include "../algorithms/IAlgorithm.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "game/Logger.h"

class GameManager {
private:
    GameBoard board;
    std::shared_ptr<Tank> tank1;
    std::shared_ptr<Tank> tank2;
    Logger logger;
//    std::unique_ptr<IAlgorithm> algo1;
//    std::unique_ptr<IAlgorithm> algo2;
    int stepCount;
    std::vector<std::shared_ptr<Shell>> shells;

public:
    GameManager(const std::string& inputFile);
    void runGame();
    void executeStep();
    void logState() const;
    void doAction(Action act, std::shared_ptr<Tank> tank);
    bool shoot(std::shared_ptr<Tank> tank);
    void move(std::shared_ptr<MovingElement> elem, bool bkwd = false);
    bool moveFwd(std::shared_ptr<Tank> tank);
    bool moveBkwd(std::shared_ptr<Tank> tank);
    void checkMovingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2);
};

#endif // GAME_MANAGER_H
