#ifndef ASGN3_GAMEMANAGER_322213836_212054837_H
#define ASGN3_GAMEMANAGER_322213836_212054837_H

#include "../Common/AbstractGameManager.h"
#include "../Common/GameResult.h"
#include "../Common/BattleInfo.h"
#include "../Common/ActionRequest.h"
#include "../Common/SatelliteView.h"
#include "../Common/Player.h"
#include "../Common/TankAlgorithm.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "GameBoard.h"
#include "Logger.h"
#include "../UserCommon/utils/Position.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

namespace GameManager_322213836_212054837 {

using GameObjectPtr = std::shared_ptr<GameObject>;

class GameManager_322213836_212054837 : AbstractGameManager{
    private:
        GameBoard board{0, 0};  // Initialize with 0x0 dimensions, will be properly set in readBoard
        Logger logger;
        bool verbose;
        std::vector<std::unique_ptr<Player>> players;  // Vector of all players
        std::vector<std::shared_ptr<Tank>> allTanks;  // Vector of all tanks
        std::map<int, int> tankCountPerPlayer;  // Track number of tanks per player for algorithm creation
        std::vector<std::shared_ptr<Shell>> shells;
        std::map<int, int> aliveTanksPerPlayer;  // Track number of alive tanks per player
        int stepCount = 0;
        int maxSteps = 0;
        int stalemateSteps = STALEMATE_STEPS;
        int totalShells = 0;  // Track total shells in the game
        bool validGame = false;

        void executeTanksStep();

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
        void runGame();
        // bool readBoard(const std::string& inputFile); // TODO: move to simulator

    public:
        GameManager_322213836_212054837(bool verbose) : verbose(verbose);
        GameResult run(size_t map_width, size_t map_height, SatelliteView& map, // <= assume it is a snapshot, NOT updated
                       size_t max_steps, size_t num_shells,
                       Player& player1, Player& player2,
                       TankAlgorithmFactory player1_tank_algo_factory,
                       TankAlgorithmFactory player2_tank_algo_factory) override;
    };


} // namespace GameManager_322213836_212054837

#endif // ASGN3_GAMEMANAGER_322213836_212054837_H 