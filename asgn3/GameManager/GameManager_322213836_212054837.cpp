#include "GameManager_322213836_212054837.h"
#include "../UserCommon/utils/definitions.h"
#include "../Common/Player.h"
#include "../Common/TankAlgorithm.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <GameManagerRegistration.h>

namespace GameManager_322213836_212054837 {

using std::cerr;
using std::endl;
    REGISTER_GAME_MANAGER(GameManager_322213836_212054837);
std::string GameManager_322213836_212054837::actionToString(ActionRequest action) const {
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

bool GameManager_322213836_212054837::isPlayerTurn() const {
    return stepCount % 2 == 0;  // Players only act on even steps
}

bool GameManager_322213836_212054837::isMaxStepsReached() {
    if (getGameStep() >= maxSteps) {
        std::string result = "Tie, reached max steps = " + std::to_string(maxSteps);
        for (const auto& [playerId, tankCount] : aliveTanksPerPlayer) {
            result += ", player " + std::to_string(playerId) + " has " + std::to_string(tankCount) + " tanks";
        }
        logger.logResult(result);
        return true;
    }
    return false;
}

bool GameManager_322213836_212054837::isStalemate() {
    if (stalemateSteps == 0) {
        logger.logResult("Tie, both players have zero shells for " + std::to_string(STALEMATE_STEPS) + " steps");
        return true;
    }
    return false;
}

std::map<int, int> GameManager_322213836_212054837::getPlayerTankCounts() const {
    return aliveTanksPerPlayer;
}

bool GameManager_322213836_212054837::isGameOver() {
    if (isMaxStepsReached()) return true;
    if (isStalemate()) return true;

    bool anyPlayerHasTanks = false;
    int playersWithTanks = 0;

    for (const auto& [playerId, tankCount] : aliveTanksPerPlayer) {
        if (tankCount > 0) {
            anyPlayerHasTanks = true;
            playersWithTanks++;
        }
    }

    if (!anyPlayerHasTanks) {
        logger.logResult("Tie, both players have zero tanks");
        return true;
    }

    if (playersWithTanks == 1) {
        // Find the winning player
        for (const auto& [playerId, tankCount] : aliveTanksPerPlayer) {
            if (tankCount > 0) {
                logger.logResult("Player " + std::to_string(playerId) + " won with " + std::to_string(tankCount) + " tanks still alive");
                return true;
            }
        }
    }

    return false;  // game continues
}

void GameManager_322213836_212054837::runGame() {
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
        if(isPlayerTurn()){
            logger.logActions();
            logger.clearActions();
        }
        ++stepCount;
    }
    logger.finalize();
}

void GameManager_322213836_212054837::moveShells() {
    std::vector<std::shared_ptr<Shell>> updatedShells;

    for (auto& shell : shells) {
        if (!shell || shell->isDestroyed()) continue;

        move(shell, false);  // ✅ move forward one half-step
        updatedShells.push_back(shell);  // keep shell alive for next frame
    }

    shells = std::move(updatedShells);
}

void GameManager_322213836_212054837::decreaseTanksCooldown() {
    for (const auto& tank : allTanks) {
        if (tank && !tank->isDestroyed()) {
            tank->decreaseShootCooldown();
        }
    }
}

ActionRequest GameManager_322213836_212054837::requestTankAction(const std::shared_ptr<Tank>& tank) {
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
            logger.addAction(actionToString(action));
            tank->setForward();
            return ActionRequest::DoNothing;
        }
        return ActionRequest::DoNothing;
    }
    return action;
}

bool GameManager_322213836_212054837::validateAndLogAction(const std::shared_ptr<Tank>& tank, ActionRequest action) {
    tank->setPrevPos();
    if (!isActionLegal(action, tank)) {
        logger.addAction(actionToString(action) + " (ignored)");
        return false;
    }

    if (action != ActionRequest::MoveBackward) {
        logger.addAction(actionToString(action));
        tank->setForward();
    }
    return true;
}

void GameManager_322213836_212054837::executeAction(const std::shared_ptr<Tank>& tank, ActionRequest action) {
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

            try {
                players[pid - 1]->updateTankWithBattleInfo(*tank->getAlgorithm(), *satelliteView);
            } catch (const std::exception& e) {
            } catch (...) {
            }
            break;
        }
        case ActionRequest::DoNothing:
            break;
        default:
            break;
    }
}

void GameManager_322213836_212054837::executeTanksStep() {
    logger.clearActions();

    // Update cooldown for all tanks
    decreaseTanksCooldown();

    // Handle all tanks' actions in a single pass
    for (const auto& tank : allTanks) {
        if (!tank || tank->isDestroyed()) {
            logger.addAction("killed");
            continue;
        }

        // Get action from the tank's algorithm
        ActionRequest action = requestTankAction(tank);
        if (action == ActionRequest::DoNothing) {
            logger.addAction(actionToString(action));
            continue;
        }

        // Validate and log the action
        if (!validateAndLogAction(tank, action)) continue;

        // Execute the action
        executeAction(tank, action);
    }
}

Position GameManager_322213836_212054837::getNextPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd) {
    return board.wrap(elem->getNextPos(bkwd));
}

bool GameManager_322213836_212054837::canMove(std::shared_ptr<Tank> tank, bool bkwd) {
    Position next = getNextPosOnBoard(tank, bkwd);
    if(board.isWall(next)){
        if (bkwd) {
            tank->setForward();
        }
        return false;  // 🚧 Wall is in the way!
    }

    return true;  // ✅ No wall → we can move
}

void GameManager_322213836_212054837::move(std::shared_ptr<MovingElement> elem, bool bkwd){
    Position newPos = getNextPosOnBoard(elem, bkwd);
    board.removeSpecificObject(elem);  // move tank
    elem->setPrevPos();
    elem->setPosition(newPos);
    board.placeObject(elem);
}

bool GameManager_322213836_212054837::checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2){
    Position p1 = elem1->getPosition();
    Position p2 = elem2->getPosition();

    Position prev1 = elem1->getPrevPos();
    Position prev2 = elem2->getPrevPos();

    // If they swapped positions between last turn and now
    return (p1 == prev2 && p2 == prev1);
}

void GameManager_322213836_212054837::checkTankCollisions(std::shared_ptr<Tank> tank, std::unordered_set<GameObjectPtr>& marked) {
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

void GameManager_322213836_212054837::checkShellCollisions(std::shared_ptr<Shell> shell, std::unordered_set<GameObjectPtr>& marked) {
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

bool GameManager_322213836_212054837::isActionLegal(ActionRequest act, std::shared_ptr<Tank> tank) {
    switch (act) {
        case ActionRequest::MoveForward:
            return canMove(tank, false);
        case ActionRequest::MoveBackward: return canMove(tank, true);
        case ActionRequest::Shoot: return canTankShoot(tank);
        default: return true;
    }
}

bool GameManager_322213836_212054837::canTankShoot(std::shared_ptr<Tank> tank){
    if (!tank->canShoot()){
        return false;
    }
    return true;
}

bool GameManager_322213836_212054837::shoot(const std::shared_ptr<Tank>& tank) {
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

void GameManager_322213836_212054837::destroyAndRemove(const GameObjectPtr& obj) {
    if (!obj || obj->isDestroyed()) return;

    obj->destroy();  // calls GameObject's destroy logic

    if (obj->isDestroyed()) {
        if (auto tank = std::dynamic_pointer_cast<Tank>(obj)) {
            int playerId = tank->getPlayerId();
            if (aliveTanksPerPlayer[playerId] > 0) {
                aliveTanksPerPlayer[playerId]--;
            }
            logger.appendToAction(tank->getGlobalIndex(), " (killed)");
            totalShells -= tank->getShellsLeft();
        }
        board.removeSpecificObject(obj);  // ✅ remove only if marked destroyed
    }
}

std::unique_ptr<SatelliteView> GameManager_322213836_212054837::getSatelliteView(const std::shared_ptr<Tank>& tank) const {
    auto boardMat = board.getBoardMat();
    Position pos = tank->getPosition();

    if (pos.y < board.getHeight() && pos.x < board.getWidth()) {
        boardMat[pos.x][pos.y] = CURR_TANK;
    }

    return std::make_unique<BoardSatelliteView>(std::move(boardMat));
}

bool GameManager_322213836_212054837::readBoardHeader(std::ifstream& file, int& numShells, int& rows, int& cols) {
    std::string line;
    std::string mapName;

    // Read map name/description (Line 1)
    if (!std::getline(file, mapName)) {
        logger.logInputError("Empty file");
        return false;
    }

    // Helper function to remove spaces from a string
    auto removeSpaces = [](std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    };

    // Read MaxSteps (Line 2)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing MaxSteps line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "MaxSteps=%d", &maxSteps) != 1) {
        logger.logInputError("Invalid MaxSteps format");
        return false;
    }

    // Read NumShells (Line 3)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing NumShells line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%d", &numShells) != 1) {
        logger.logInputError("Invalid NumShells format");
        return false;
    }

    // Read Rows (Line 4)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing Rows line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%d", &rows) != 1) {
        logger.logInputError("Invalid Rows format");
        return false;
    }

    // Read Cols (Line 5)
    if (!std::getline(file, line)) {
        logger.logInputError("Missing Cols line");
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%d", &cols) != 1) {
        logger.logInputError("Invalid Cols format");
        return false;
    }

    // Validate dimensions
    if (rows <= 0 || cols <= 0) {
        logger.logInputError("Invalid dimensions");
        return false;
    }

    return true;
}

bool GameManager_322213836_212054837::processMapCell(char cell, const Position& pos, int numShells) {
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
                auto algorithm = algorithmFactory.create(playerId, tankIdx);
                tank->setAlgorithm(std::move(algorithm));

                // Initialize alive tanks counter for this player
                aliveTanksPerPlayer[playerId]++;
            }
            break;
    }
    return true;
}

bool GameManager_322213836_212054837::readMapContent(std::ifstream& file, int rows, int cols, int numShells) {
    // Read the map content
    std::vector<std::string> mapLines;
    std::string line;
    while (std::getline(file, line)) {
        mapLines.push_back(line);
    }

    for (int y = 0; y < rows; y++) {
        std::string currentLine = (y < static_cast<int>(mapLines.size())) ? mapLines[y] : "";

        for (int x = 0; x < cols; x++) {
            char cell = (x < static_cast<int>(currentLine.length())) ? currentLine[x] : EMPTY;
            Position pos(x, y);

            if (!processMapCell(cell, pos, numShells)) {
                return false;
            }
        }
    }

    // Create players for each player ID that has tanks
    players.clear();
    players.resize(2);  // Resize to hold 2 players
    for (const auto& [playerId, tankCount] : aliveTanksPerPlayer) {
        players[playerId - 1] = playerFactory.create(playerId, cols, rows, maxSteps, numShells);
    }

    // Check if we have any players with tanks
    if (players.empty()) {
        logger.logInputError("No tanks found on the board");
        return false;
    }

    // If there's only one player with tanks, they win immediately
    if (players.size() == 1) {
        // Find which player exists (should be at globalIndex 0 or 1)
        int playerId = players[0] ? 1 : 2;
        logger.logResult("Player " + std::to_string(playerId) + " won with " + std::to_string(aliveTanksPerPlayer[playerId]) + " tanks still alive");
        return false;  // Don't start the game since it's already over
    }

    // Initialize total shells count
    totalShells = allTanks.size() * numShells;

    return true;
}

bool GameManager_322213836_212054837::readBoard(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        logger.logInputError("Could not open file " + inputFile);
        cerr << "Could not open file " + inputFile << endl;
        return false;
    }

    int numShells = 0;
    int rows = 0;
    int cols = 0;

    // Read and validate header information
    if (!readBoardHeader(file, numShells, rows, cols)) {
        return false;
    }

    // Initialize the board with the specified dimensions
    board = GameBoard(cols, rows);

    // Read and process map content
    if (!readMapContent(file, rows, cols, numShells)) {
        return false;
    }

    // If we got here, everything was successful
    validGame = true;
    return true;
}

int GameManager_322213836_212054837::getGameStep() const {
    return stepCount / 2;  // Each game step consists of two stepCount increments
}

GameResult GameManager_322213836_212054837::run(size_t map_width, size_t map_height, SatelliteView &map, size_t max_steps, size_t num_shells,
                                                Player &player1, Player &player2, TankAlgorithmFactory player1_tank_algo_factory,
                                                TankAlgorithmFactory player2_tank_algo_factory) {
        return GameResult();
    }

} // namespace GameManager_322213836_212054837