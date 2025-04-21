#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H


#include "GameBoard.h"
//#include "../algorithms/IAlgorithm.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "game/Logger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_set>

#include "objects/Tank.h"
#include "objects/Wall.h"
#include "objects/Shell.h"
#include "objects/Mine.h"
#include "game/Action.h"
#include "definitions.h"

class GameManager {
private:
    GameBoard board;
    Logger logger;
//    std::unique_ptr<IAlgorithm> algo1;
//    std::unique_ptr<IAlgorithm> algo2;
    int stepCount;
    std::vector<std::shared_ptr<Shell>> shells;
    std::vector<std::shared_ptr<Tank>> tanks;
    int stepsRemaining = STALEMATE_STEPS;

public:
    GameManager(const std::string& inputFile);
    void runGame();
    void executeTanksStep();
    void logState() const;
    bool shoot(std::shared_ptr<Tank> tank);
    void move(std::shared_ptr<MovingElement> elem, bool bkwd);
    Position getPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd);
    void moveShells();
    bool checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2);
    bool canMove(std::shared_ptr<MovingElement> elem, bool bkwd);
    bool isActionLegal(Action act, std::shared_ptr<Tank> tank);
    bool isPlayerTurn();
    void destroyAndRemove(const GameObjectPtr &obj);

    void checkShellCollisions(std::shared_ptr<Shell> shell, std::unordered_set<GameObjectPtr> &marked);

    void checkTankCollisions(std::shared_ptr<Tank> tank, std::unordered_set<GameObjectPtr> &marked);

    bool areAllTanksOutOfAmmo() const;

    bool isGameOver() const;

    std::string actionToString(Action action) const;
};

#endif // GAME_MANAGER_H
