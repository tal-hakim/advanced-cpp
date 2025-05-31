#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "GameBoard.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "game/Logger.h"
#include "game/BoardSatelliteView.h"
#include "../../common/PlayerFactory.h"
#include "../../common/TankAlgorithmFactory.h"
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
    const PlayerFactory& playerFactory;
    const TankAlgorithmFactory& algorithmFactory;
    GameBoard board{0, 0};  // Initialize with 0x0 dimensions, will be properly set in readBoard
    Logger logger;
    std::vector<std::unique_ptr<Player>> players;  // Vector of all players
    std::map<int, std::vector<std::shared_ptr<Tank>>> playersArmy;  // Map of player_id to their tanks
    std::vector<std::shared_ptr<Shell>> shells;
    std::map<int, int> aliveTanksPerPlayer;  // Track number of alive tanks per player
    int stepCount = 0;
    int maxSteps = 0;
    int stalemateSteps = STALEMATE_STEPS;
    int totalShells = 0;  // Track total shells in the game
    bool validGame = false;

    void executeTanksStep();
    void logState();
    bool shoot(const std::shared_ptr<Tank>& tank);
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
    bool isGameOver();
    std::string actionToString(ActionRequest action) const;
    bool canTankShoot(std::shared_ptr<Tank> tank);
    int getGameStep() const;
    std::unique_ptr<SatelliteView> getSatelliteView(const std::shared_ptr<Tank>& tank) const;
    int getNextPlayerIndex() const;
    void decreaseTanksCooldown();
    ActionRequest requestTankAction(const std::shared_ptr<Tank>& tank);
    void executeAction(const std::shared_ptr<Tank>& tank, ActionRequest action);
    bool validateAndLogAction(const std::shared_ptr<Tank>& tank, ActionRequest action);
    bool isMaxStepsReached();
    bool isStalemate();
    std::map<int, int> getPlayerTankCounts() const;

    // Board reading helper functions
    bool readBoardHeader(std::ifstream& file, int& numShells, int& rows, int& cols);
    bool processMapCell(char cell, const Position& pos, int numShells);
    bool readMapContent(std::ifstream& file, int rows, int cols, int numShells);

public:
    GameManager(const PlayerFactory& playerFactory, const TankAlgorithmFactory& algorithmFactory, const std::string& inputFile);
    void runGame();
    bool readBoard(const std::string& inputFile);
};

#endif // GAME_MANAGER_H
