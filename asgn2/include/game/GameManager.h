#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "GameBoard.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "game/Logger.h"
#include "game/BoardSatelliteView.h"
#include "../../common/PlayerFactory.h"
#include "../../common/TankAlgorithmFactory.h"
#include "Player.h"
#include "../../common/SatelliteView.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <vector>
#include <map>

class GameManager {
private:
    GameBoard board;
    Logger logger;
    std::vector<std::unique_ptr<Player>> players;  // Vector of all players
    std::map<int, std::vector<std::shared_ptr<Tank>>> playersArmy;  // Map of player_id to their tanks
    std::vector<std::shared_ptr<Shell>> shells;
    int stepCount = 0;
    int maxSteps = 0;
    int stalemateSteps = STALEMATE_STEPS;
    bool validGame = true;
    int currentPlayerIndex = 0;  // Track current player's turn

public:
    GameManager(const PlayerFactory& playerFactory, const TankAlgorithmFactory& algorithmFactory);
    void runGame();
    void executeTanksStep();
    void logState() const;
    bool shoot(std::shared_ptr<Tank> tank);
    void move(std::shared_ptr<MovingElement> elem, bool bkwd);
    Position getNextPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd);
    void moveShells();
    bool checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2);
    bool canMove(std::shared_ptr<Tank> tank, bool bkwd);
    bool isActionLegal(ActionRequest act, std::shared_ptr<Tank> tank);
    bool isPlayerTurn() const;
    void destroyAndRemove(const GameObjectPtr &obj);
    void checkShellCollisions(std::shared_ptr<Shell> shell, std::unordered_set<GameObjectPtr> &marked);
    void checkTankCollisions(std::shared_ptr<Tank> tank, std::unordered_set<GameObjectPtr> &marked);
    bool areAllTanksOutOfAmmo() const;
    bool isGameOver();
    std::string actionToString(ActionRequest action) const;
    bool canTankShoot(std::shared_ptr<Tank> tank);
    int getGameStep() const { return stepCount / players.size(); }
    std::unique_ptr<SatelliteView> getSatelliteView(const Tank& tank) const;
    bool readBoard(const std::string& inputFile);
    int getNextPlayerIndex() const;
};

#endif // GAME_MANAGER_H
