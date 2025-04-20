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
    std::vector<std::shared_ptr<Tank>> tanks;

public:
    GameManager(const std::string& inputFile);
    void runGame();
    void executeTanksStep();
    void logState() const;
    void doAction(Action act, std::shared_ptr<Tank> tank);
    bool shoot(std::shared_ptr<Tank> tank);
    void move(std::shared_ptr<MovingElement> elem, bool bkwd);
    Position getPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd);
    void moveShells();
    bool moveFwd(std::shared_ptr<Tank> tank);
    bool moveBkwd(std::shared_ptr<Tank> tank);
    void checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2);
    bool canMove(std::shared_ptr<MovingElement> elem, bool bkwd);
    bool isActionLegal(Action act, std::shared_ptr<Tank> tank);

    void checkCollision();

    void updateBoard();

    void moveObjects(const std::vector<std::shared_ptr<MovingElement>> &objects);

    void updateTanks();

    void updateShells();

    bool isPlayerTurn();
};

#endif // GAME_MANAGER_H
