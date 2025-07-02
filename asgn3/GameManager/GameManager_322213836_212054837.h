#ifndef ASGN3_GAMEMANAGER_322213836_212054837_H
#define ASGN3_GAMEMANAGER_322213836_212054837_H

#include "../common/AbstractGameManager.h"
#include "../common/GameResult.h"
#include "../common/BattleInfo.h"
#include "../common/ActionRequest.h"
#include "../common/SatelliteView.h"
#include "../common/Player.h"
#include "../common/TankAlgorithm.h"
#include "objects/Tank.h"
#include "objects/Shell.h"
#include "objects/Wall.h"
#include "objects/Mine.h"
#include "GameBoard.h"
#include "../UserCommon/BoardSatelliteView.h"
#include "Logger.h"
#include "../UserCommon/utils/Position.h"
#include "../UserCommon/utils/timeUtil.h"
#include "../common/GameManagerRegistration.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <unordered_set>


namespace GameManager_322213836_212054837 {

    using GameObjectPtr = std::shared_ptr<GameObject>;

    class GameManager : public AbstractGameManager{
        private:
            GameBoard board{0, 0};  // Initialize with 0x0 dimensions, will be properly set in readBoard
            Logger logger;
            bool verbose;
            std::vector<Player*> players;  // Vector of all players
            std::vector<std::shared_ptr<Tank>> allTanks;  // Vector of all tanks
            std::map<int, int> tankCountPerPlayer;  // Track number of tanks per player for algorithm creation
            std::vector<std::shared_ptr<Shell>> shells;
            std::vector<size_t> aliveTanksPerPlayer;  // Track number of alive tanks per player
            size_t stepCount = 0;
            size_t maxSteps = 0;
            int stalemateSteps = STALEMATE_STEPS;
            size_t totalShells = 0;  // Track total shells in the game
            bool validGame = true;
            GameResult finalResult;
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
            size_t getGameStep() const;
            std::unique_ptr<SatelliteView> getSatelliteView(const std::shared_ptr<Tank>& tank) const;
            void decreaseTanksCooldown();
            ActionRequest requestTankAction(const std::shared_ptr<Tank>& tank);
            void executeAction(const std::shared_ptr<Tank>& tank, ActionRequest action);
            bool validateAndLogAction(const std::shared_ptr<Tank>& tank, ActionRequest action);
            bool isMaxStepsReached();
            bool isStalemate();
            std::vector<size_t> getPlayerTankCounts() const;

            // Board reading helper functions
            void processMapCell(char cell, const Position& pos, size_t numShells, TankAlgorithmFactory& algOneFactory, TankAlgorithmFactory& algTwoFactory);
            void
            readSatelliteView(const SatelliteView& view, size_t numShells, TankAlgorithmFactory& algOneFactory, TankAlgorithmFactory& algTwoFactory);
            void runGame();

        public:
            GameManager(bool verbose) : verbose(verbose) {
            };
            ~GameManager() = default;
            GameResult run( size_t map_width, size_t map_height,
                            const SatelliteView& map, // <= a snapshot, NOT updated
                            string map_name,
                            size_t max_steps, size_t num_shells,
                            Player& player1, string name1, Player& player2, string name2,
                            TankAlgorithmFactory player1_tank_algo_factory,
                            TankAlgorithmFactory player2_tank_algo_factory) override;

        };


} // namespace GameManager

#endif // ASGN3_GAMEMANAGER_322213836_212054837_H 