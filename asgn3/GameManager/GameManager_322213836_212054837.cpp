#include "GameManager_322213836_212054837.h"
#include "../UserCommon/utils/definitions.h"
#include "../common/Player.h"
#include "../common/TankAlgorithm.h"
#include <fstream>
#include <algorithm>
#include <iostream>

using namespace GameManager_322213836_212054837;
REGISTER_GAME_MANAGER(GameManager_212054837_322213836);

namespace GameManager_322213836_212054837 {
using namespace UserCommon_322213836_212054837;
using std::cerr;
using std::endl;
std::string GameManager_212054837_322213836::actionToString(ActionRequest action) const {
    switch (action) {
        case ActionRequest::MoveForward: return "MoveForward";
        case ActionRequest::MoveBackward: return "MoveBackwards";
        case ActionRequest::Shoot: return "Shoot";
        case ActionRequest::RotateLeft45: return "RotateLeft45";
        case ActionRequest::RotateRight45: return "RotateRight45";
        case ActionRequest::RotateLeft90: return "RotateLeft90";
        case ActionRequest::RotateRight90: return "RotateRight90";
        case ActionRequest::GetBattleInfo: return "GetBattleInfo";
        case ActionRequest::DoNothing: return "DoNothing";
        default: return "Unknown";
    }
}

bool GameManager_212054837_322213836::isPlayerTurn() const {
    return stepCount % 2 == 0;  // Players only act on even steps
}

bool GameManager_212054837_322213836::isMaxStepsReached() {
    if (getGameStep() >= maxSteps) {
        std::string result = "Tie, reached max steps = " + std::to_string(maxSteps);
        for (size_t i = 0; i < aliveTanksPerPlayer.size(); i++) {
            result += ", player " + std::to_string(i + 1) + " has " + std::to_string(aliveTanksPerPlayer[i]) + " tanks";
        }
        if (verbose) logger.logResult(result);
        return true;
    }
    return false;
}

bool GameManager_212054837_322213836::isStalemate() {
    if (stalemateSteps == 0) {
        if (verbose) logger.logResult("Tie, both players have zero shells for " + std::to_string(STALEMATE_STEPS) + " steps");
        return true;
    }
    return false;
}

std::vector<size_t> GameManager_212054837_322213836::getPlayerTankCounts() const {
    return aliveTanksPerPlayer;
}

bool GameManager_212054837_322213836::isGameOver() {
    if (isMaxStepsReached()) {
        finalResult = {TIE, GameResult::Reason::MAX_STEPS, aliveTanksPerPlayer, getSatelliteView(nullptr), static_cast<size_t>(getGameStep())};
        return true;
    }
    if (isStalemate()) {
        finalResult = {TIE, GameResult::Reason::ZERO_SHELLS, aliveTanksPerPlayer, getSatelliteView(nullptr), getGameStep()};
        return true;
    }

    bool anyPlayerHasTanks = false;
    int playersWithTanks = 0;

    for (size_t i = 0; i < aliveTanksPerPlayer.size(); i++) {
        auto tankCount = aliveTanksPerPlayer[i];
        if (tankCount > 0) {
            anyPlayerHasTanks = true;
            playersWithTanks++;
        }
    }

    if (!anyPlayerHasTanks) {
        finalResult = {TIE, GameResult::Reason::ALL_TANKS_DEAD, aliveTanksPerPlayer, getSatelliteView(nullptr), getGameStep()};
        if (verbose) logger.logResult("Tie, both players have zero tanks");
        return true;
    }

    if (playersWithTanks == 1) {
        // Find the winning player
        for (size_t i = 0; i < aliveTanksPerPlayer.size(); i++) {
            auto playerId = i + 1;
            auto tankCount = aliveTanksPerPlayer[i];
            if (tankCount > 0) {
                finalResult = {(int)playerId, GameResult::Reason::ALL_TANKS_DEAD, aliveTanksPerPlayer, getSatelliteView(nullptr), getGameStep()};
                if (verbose) logger.logResult("Player " + std::to_string(playerId) + " won with " + std::to_string(tankCount) + " tanks still alive");
                return true;
            }
        }
    }

    return false;  // game continues
}

void GameManager_212054837_322213836::runGame() {
    if (!validGame) {
        return;
    }
    stalemateSteps = UNINITIALIZED;  // Initialize stalemate steps
    while (!isGameOver()) {
        std::unordered_set<GameObjectPtr> markedForDestruction;
        moveShells();

        // check shells collisions
        for (const auto& shell : shells) {
            if (shell && !shell->isDestroyed()) checkShellCollisions(shell, markedForDestruction);
        }

        if (isPlayerTurn()) {  // On even steps, all players act simultaneously
            if (stalemateSteps > 0) stalemateSteps--;
            executeTanksStep();

            // check tanks collisions
            for (const auto& tank : allTanks) {
                if (tank && !tank->isDestroyed()) checkTankCollisions(tank, markedForDestruction);
            }

            for (const auto& shell : shells) {
                if (shell && !shell->isDestroyed()) checkShellCollisions(shell, markedForDestruction);
            }
        }

        for (const auto& obj : markedForDestruction) {
            destroyAndRemove(obj);
        }

        if(isPlayerTurn() && verbose){
//            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
//            std::cout << "Game Step" << getGameStep() << std::endl;
//            board.printBoard();
            logger.logActions();
            logger.clearActions();
        }
        ++stepCount;
    }
    if (verbose) logger.finalize();
}

void GameManager_212054837_322213836::moveShells() {
    std::vector<std::shared_ptr<Shell>> updatedShells;

    for (auto& shell : shells) {
        if (!shell || shell->isDestroyed()) continue;

        move(shell, false);  // ✅ move forward one half-step
        updatedShells.push_back(shell);  // keep shell alive for next frame
    }

    shells = std::move(updatedShells);
}

void GameManager_212054837_322213836::decreaseTanksCooldown() {
    for (const auto& tank : allTanks) {
        if (tank && !tank->isDestroyed()) {
            tank->decreaseShootCooldown();
        }
    }
}

ActionRequest GameManager_212054837_322213836::requestTankAction(const std::shared_ptr<Tank>& tank) {
    auto action = tank->getAlgorithm()->getAction();
    if (!tank->isGoingBack()) {
        return action;
    }

    tank->decreaseBackwardTimer();
    if (!tank->isLastStepBack(getGameStep())) {
        if (tank->getBackwardTimer() == 0) {
            return ActionRequest::MoveBackward;
        }
        else if (action == ActionRequest::MoveForward) {
            if (verbose) logger.addAction(actionToString(action));
            tank->setForward();
            return ActionRequest::DoNothing;
        }
        return ActionRequest::DoNothing;
    }
    return action;
}

bool GameManager_212054837_322213836::validateAndLogAction(const std::shared_ptr<Tank>& tank, ActionRequest action) {
    tank->setPrevPos();
    if (!isActionLegal(action, tank)) {
        if (verbose) logger.addAction(actionToString(action) + " (ignored)");
        return false;
    }

    if (action != ActionRequest::MoveBackward) {
        if (verbose) logger.addAction(actionToString(action));
        tank->setForward();
    }
    return true;
}

void GameManager_212054837_322213836::executeAction(const std::shared_ptr<Tank>& tank, ActionRequest action) {
    std::unique_ptr<SatelliteView> satelliteView;  // Move declaration outside switch
    switch (action) {
        case ActionRequest::MoveForward:
            if (canMove(tank, false)) {
                move(tank, false);
            }
            break;
        case ActionRequest::MoveBackward:
            if (canMove(tank, true)) {
                move(tank, true);
            }
            break;
        case ActionRequest::RotateLeft90:
            tank->rotate(-2);
            break;
        case ActionRequest::RotateRight90:
            tank->rotate(2);
            break;
        case ActionRequest::RotateLeft45:
            tank->rotate(-1);
            break;
        case ActionRequest::RotateRight45:
            tank->rotate(1);
            break;
        case ActionRequest::Shoot:
            if (canTankShoot(tank)) {
                shoot(tank);
            }
            break;
        case ActionRequest::GetBattleInfo:
        {
            satelliteView = getSatelliteView(tank);
            int pid = tank->getPlayerId();  // Should be 1 or 2

            if (pid < 1 || pid > 2) {
                break;
            }
            if (pid - 1 >= static_cast<int>(players.size())) {
                break;
            }
            if (!players[pid - 1]) {
                break;
            }

            players[pid - 1]->updateTankWithBattleInfo(*tank->getAlgorithm(), *satelliteView);

            break;
        }
        case ActionRequest::DoNothing:
            break;
        default:
            break;
    }
}

void GameManager_212054837_322213836::executeTanksStep() {
    if (verbose) logger.clearActions();

    // Update cooldown for all tanks
    decreaseTanksCooldown();

    // Handle all tanks' actions in a single pass
    for (const auto& tank : allTanks) {
        if (!tank || tank->isDestroyed()) {
            if (verbose) logger.addAction("killed");
            continue;
        }

        // Get action from the tank's algorithm
        ActionRequest action = requestTankAction(tank);
        if (action == ActionRequest::DoNothing) {
            if (verbose) logger.addAction(actionToString(action));
            continue;
        }

        // Validate and log the action
        if (!validateAndLogAction(tank, action)) continue;

        // Execute the action
        executeAction(tank, action);
    }
}

Position GameManager_212054837_322213836::getNextPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd) {
    return board.wrap(elem->getNextPos(bkwd));
}

bool GameManager_212054837_322213836::canMove(std::shared_ptr<Tank> tank, bool bkwd) {
    Position next = getNextPosOnBoard(tank, bkwd);
    if(board.isWall(next)){
        if (bkwd) {
            tank->setForward();
        }
        return false;  // 🚧 Wall is in the way!
    }

    return true;  // ✅ No wall → we can move
}

void GameManager_212054837_322213836::move(std::shared_ptr<MovingElement> elem, bool bkwd){
    Position newPos = getNextPosOnBoard(elem, bkwd);
    board.removeSpecificObject(elem);  // move tank
    elem->setPrevPos();
    elem->setPosition(newPos);
    board.placeObject(elem);
}

bool GameManager_212054837_322213836::checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2){
    Position p1 = elem1->getPosition();
    Position p2 = elem2->getPosition();

    Position prev1 = elem1->getPrevPos();
    Position prev2 = elem2->getPrevPos();

    // If they swapped positions between last turn and now
    return (p1 == prev2 && p2 == prev1);
}

void GameManager_212054837_322213836::checkTankCollisions(std::shared_ptr<Tank> tank, std::unordered_set<GameObjectPtr>& marked) {
    if (!tank) return;

    Position currPos = tank->getPosition();

    // 1. Passing collision with other tanks
    for (const auto& other : allTanks) {
        if (!other || other == tank || other->isDestroyed()) continue;

        if (checkPassingCollision(tank, other)) {
            tank->setCollisionType(other->toString());
            other->setCollisionType(tank->toString());
            marked.insert(tank);
            marked.insert(other);
        }
    }

    // 2. Passing collision with shells
    for (const auto& shell : shells) {
        if (!shell || shell->isDestroyed()) continue;

        if (checkPassingCollision(tank, shell)) {
            tank->setCollisionType(shell->toString());
            marked.insert(tank);
            marked.insert(shell);
        }
    }

    // 3. Static collision at current position (excluding self)
    for (const auto& obj : board.getObjectsAt(currPos)) {
        if (!obj || obj == tank) continue;
        tank->setCollisionType(obj->toString());
        marked.insert(obj);
        marked.insert(tank);
    }
}

void GameManager_212054837_322213836::checkShellCollisions(std::shared_ptr<Shell> shell, std::unordered_set<GameObjectPtr>& marked) {
    if (!shell || shell->isDestroyed()) return;

    Position currPos = shell->getPosition();

    // 1. Check passing collisions with other shells
    for (const auto& other : shells) {
        if (!other || other == shell || other->isDestroyed()) continue;

        if (checkPassingCollision(shell, other)) {
            marked.insert(other);
        }
    }

    // 2. Check for non-mine, non-self collisions in current position
    for (const auto& obj : board.getObjectsAt(currPos)) {
        if (!obj || obj == shell) continue;
        if (std::dynamic_pointer_cast<Mine>(obj)) continue;
        if (std::dynamic_pointer_cast<Tank>(obj)) {
            auto tempTank = std::dynamic_pointer_cast<Tank>(obj);
            tempTank->setCollisionType(shell->toString());
        }
        marked.insert(obj);
        marked.insert(shell);  // also mark the shell itself if it collided
    }
}

bool GameManager_212054837_322213836::isActionLegal(ActionRequest act, std::shared_ptr<Tank> tank) {
    switch (act) {
        case ActionRequest::MoveForward:
            return canMove(tank, false);
        case ActionRequest::MoveBackward: return canMove(tank, true);
        case ActionRequest::Shoot: return canTankShoot(tank);
        default: return true;
    }
}

bool GameManager_212054837_322213836::canTankShoot(std::shared_ptr<Tank> tank){
    if (!tank->canShoot()){
        return false;
    }
    return true;
}

bool GameManager_212054837_322213836::shoot(const std::shared_ptr<Tank>& tank) {
    Position start = tank->getPosition();
    Direction dir = tank->getDirection();
    int shooterId = tank->getPlayerId();

    auto shell = std::make_shared<Shell>(start, dir, shooterId);

    // Add shell to game state
    shells.push_back(shell);

    // Optionally: place it on the boardView (if you want it visible on the grid)
    board.placeObject(shell);
    move(shell, false);
    shell->setPrevPos();
    tank->shoot();

    // Decrease total shells count
    totalShells--;

    // Check if this was the last shell
    if (totalShells == 0 && stalemateSteps == UNINITIALIZED) {
        stalemateSteps = STALEMATE_STEPS;
    }

    return true;
}

void GameManager_212054837_322213836::destroyAndRemove(const GameObjectPtr& obj) {
    if (!obj || obj->isDestroyed()) return;

    obj->destroy();  // calls GameObject's destroy logic

    if (obj->isDestroyed()) {
        if (auto tank = std::dynamic_pointer_cast<Tank>(obj)) {
            int playerId = tank->getPlayerId();
            if (aliveTanksPerPlayer[playerId - 1] > 0) {
                aliveTanksPerPlayer[playerId - 1]--;
            }
            if (verbose) logger.appendToAction(tank->getGlobalIndex(), " (killed)");
            totalShells -= tank->getShellsLeft();
        }
        board.removeSpecificObject(obj);  // ✅ remove only if marked destroyed
    }
}

std::unique_ptr<SatelliteView> GameManager_212054837_322213836::getSatelliteView(const std::shared_ptr<Tank>& tank) const {
    auto boardMat = board.getBoardMat();

    if (tank) { // Only if tank is provided
        Position pos = tank->getPosition();

        if (pos.y < static_cast<int>(board.getHeight()) && pos.x < static_cast<int>(board.getWidth())) {
            boardMat[pos.x][pos.y] = CURR_TANK;
        }
    }
    return std::make_unique<BoardSatelliteView>(std::move(boardMat));
}


void GameManager_212054837_322213836::processMapCell(char cell, const Position& pos, size_t numShells, TankAlgorithmFactory& algOneFactory, TankAlgorithmFactory& algTwoFactory) {
    switch (cell) {
        case WALL: // Wall
            board.placeObject(std::make_shared<Wall>(pos));
            break;
        case MINE: // Mine
            board.placeObject(std::make_shared<Mine>(pos));
            break;
        case EMPTY: // Empty space
            break;
        default: // Tank or other character
            if (cell == PLAYER1_TANK || cell == PLAYER2_TANK) {  // Support for 2 players
                int playerId = cell - '0';

                // Create tank with appropriate direction
                Direction initialDir = (playerId == PLAYER_1_ID) ? Direction::L : Direction::R;
                auto tank = std::make_shared<Tank>(pos, initialDir, playerId, static_cast<int>(size(allTanks)), numShells);

                // Add to all tanks vector
                allTanks.push_back(tank);
                board.placeObject(tank);

                // Create and add algorithm for this tank
                int tankIdx = tankCountPerPlayer[playerId]++;
                auto algorithm = (playerId == PLAYER_1_ID) ? algOneFactory(playerId, tankIdx) : algTwoFactory(playerId, tankIdx);
                tank->setAlgorithm(std::move(algorithm));

                // Initialize alive tanks counter for this player
                aliveTanksPerPlayer[playerId - 1]++;
            }
            break;
    }
}

void GameManager_212054837_322213836::readSatelliteView(const SatelliteView& view, size_t numShells, TankAlgorithmFactory& algOneFactory, TankAlgorithmFactory& algTwoFactory) {
    // Read the map content

    for(size_t j = 0; j < board.getHeight() ; j++){
        for (size_t i = 0; i <board.getWidth(); ++i) {
            char cell = view.getObjectAt(i, j);
            processMapCell(cell, {i, j}, numShells, algOneFactory, algTwoFactory);
        }
    }
}


size_t GameManager_212054837_322213836::getGameStep() const {
    return stepCount / 2;  // Each game step consists of two stepCount increments
}

GameResult GameManager_212054837_322213836::run(size_t map_width, size_t map_height,
                                                const SatelliteView& map, // <= a snapshot, NOT updated
                             string map_name,
                                                size_t max_steps, size_t num_shells,
                                                Player& player1, string name1, Player& player2, string name2,
                                                TankAlgorithmFactory player1_tank_algo_factory,
                                                TankAlgorithmFactory player2_tank_algo_factory) {

    // Clear
    players.clear();
    allTanks.clear();
    shells.clear();
    aliveTanksPerPlayer.clear();
    aliveTanksPerPlayer.resize(NUM_PLAYERS, 0);
    tankCountPerPlayer.clear();
    maxSteps = max_steps;
    if (verbose){
//        logger.setLogFile(typeid(player1).name()); // TODO: name the log file
        logger.setLogFile(map_name + "_" + name1 + "_" + name2 + "_" + getCurrentTimeString());
    }
    // initialize board
    board = GameBoard(map_width, map_height);
    readSatelliteView(map, num_shells, player1_tank_algo_factory, player2_tank_algo_factory);

    // set players
    players.push_back(&player1);
    players.push_back(&player2);

    // count total shells
    totalShells = allTanks.size() * num_shells;
    // run the game
    runGame();

    return std::move(finalResult);
    }

} // namespace GameManager