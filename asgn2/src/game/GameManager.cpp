#include "game/GameManager.h"

GameManager::GameManager(const PlayerFactory& playerFactory, const TankAlgorithmFactory& algorithmFactory) {
    // Initialize empty playersArmy map
    playersArmy.clear();
}

std::string GameManager::actionToString(ActionRequest action) const {
    switch (action) {
        case ActionRequest::MoveForward: return "Move Forward";
        case ActionRequest::MoveBackward: return "Move Backwards";
        case ActionRequest::Shoot: return "Shoot";
        case ActionRequest::RotateLeft45: return "RotateLeft 1/8";
        case ActionRequest::RotateRight45: return "RotateRight 1/8";
        case ActionRequest::RotateLeft90: return "RotateLeft 1/4";
        case ActionRequest::RotateRight90: return "RotateRight 1/4";
        case ActionRequest::DoNothing: return "DoNothing";
        default: return "Unknown";
    }
}

bool GameManager::isPlayerTurn() const {
    return stepCount % 2 == 0;  // Players only act on even steps
}

int GameManager::getNextPlayerIndex() const {
    return (currentPlayerIndex + 1) % players.size();
}

bool GameManager::isGameOver() {
    if (stepCount >= maxSteps) {
        std::string result = "Tie, reached max steps = " + std::to_string(maxSteps);
        for (const auto& [playerId, tanks] : playersArmy) {
            int aliveTanks = 0;
            for (const auto& tank : tanks) {
                if (tank && !tank->isDestroyed()) aliveTanks++;
            }
            result += ", player " + std::to_string(playerId) + " has " + std::to_string(aliveTanks) + " tanks";
        }
        logger.logResult(result);
        return true;
    }
    
    if (stalemateSteps <= 0) {
        logger.logResult("Tie - no ammo for the last 40 steps");
        return true;
    }

    // Count remaining tanks for each player
    std::map<int, int> playerTankCounts;
    bool anyPlayerHasTanks = false;
    int playersWithTanks = 0;

    for (const auto& [playerId, tanks] : playersArmy) {
        int aliveTanks = 0;
        for (const auto& tank : tanks) {
            if (tank && !tank->isDestroyed()) aliveTanks++;
        }
        playerTankCounts[playerId] = aliveTanks;
        if (aliveTanks > 0) {
            anyPlayerHasTanks = true;
            playersWithTanks++;
        }
    }

    if (!anyPlayerHasTanks) {
        logger.logResult("Tie, all players have zero tanks");
        return true;
    }

    if (playersWithTanks == 1) {
        // Find the winning player
        for (const auto& [playerId, tankCount] : playerTankCounts) {
            if (tankCount > 0) {
                logger.logResult("Player " + std::to_string(playerId) + " won with " + 
                               std::to_string(tankCount) + " tanks still alive");
                return true;
            }
        }
    }

    return false;  // game continues
}

void GameManager::runGame() {
    if (!validGame) {
        return;
    }
    logger.logGameStart();
    while (!isGameOver()) {
        std::unordered_set<GameObjectPtr> markedForDestruction;
        moveShells();
        
        // check shells collisions
        for (const auto& shell : shells) {
            checkShellCollisions(shell, markedForDestruction);
        }
        
        if (isPlayerTurn()) {  // On even steps, all players act simultaneously
            if (areAllTanksOutOfAmmo()) stalemateSteps--;
            executeTanksStep();
            
            // check tanks collisions
            for (const auto& [playerId, tanks] : playersArmy) {
                for (const auto& tank : tanks) {
                    if (tank) checkTankCollisions(tank, markedForDestruction);
                }
            }
            
            for (const auto& shell : shells) {
                checkShellCollisions(shell, markedForDestruction);
            }
        }
        
        for (const auto& obj : markedForDestruction) {
            destroyAndRemove(obj);
        }
        ++stepCount;
    }
    logger.finalize();
}

bool GameManager::areAllTanksOutOfAmmo() const {
    for (const auto& [playerId, tanks] : playersArmy) {
        for (const auto& tank : tanks) {
            if (tank && !tank->isOutOfAmmo()) {
                return false;  // At least one tank has ammo
            }
        }
    }
    return true;  // All tanks are out of ammo
}

void GameManager::moveShells() {
    std::vector<std::shared_ptr<Shell>> updatedShells;

    for (auto& shell : shells) {
        if (!shell || shell->isDestroyed()) continue;

        move(shell, false);  // ✅ move forward one half-step
        updatedShells.push_back(shell);  // keep shell alive for next frame
    }

    shells = std::move(updatedShells);
}

void GameManager::executeTanksStep() {
    logger.logStepNum(getGameStep());
    int currentStep = getGameStep();
    
    // Update cooldown for all tanks
    for (const auto& [playerId, tanks] : playersArmy) {
        for (const auto& tank : tanks) {
            if (tank && !tank->isDestroyed()) {
                tank->decreaseShootCooldown();
            }
        }
    }

    // First pass: Handle GetBattleInfo actions for all players simultaneously
    for (const auto& [playerId, tanks] : playersArmy) {
        for (const auto& tank : tanks) {
            if (!tank || tank->isDestroyed()) continue;

            ActionRequest action = tank->getAlgorithm()->getAction();
            if (action == ActionRequest::GetBattleInfo) {
                // Create battle info with current step
                auto battleInfo = std::make_unique<BattleInfo>();
                battleInfo->setGameStep(currentStep);
                players[playerId - 1]->updateTankWithBattleInfo(*tank->getAlgorithm(), getSatelliteView(*tank), std::move(battleInfo));
            }
        }
    }

    // Second pass: Handle movement and combat actions for all players simultaneously
    for (const auto& [playerId, tanks] : playersArmy) {
        for (const auto& tank : tanks) {
            if (!tank || tank->isDestroyed()) continue;

            // Get action from the tank's algorithm
            ActionRequest action = tank->getAlgorithm()->getAction();
            
            // Skip if it's a GetBattleInfo action (handled in first pass)
            if (action == ActionRequest::GetBattleInfo) continue;

            if (tank->isGoingBack()) {
                tank->decreaseBackwardTimer();
                if(!tank->isLastStepBack(getGameStep())) {
                    if (tank->getBackwardTimer() == 0) {
                        action = ActionRequest::MoveBackward;
                    }
                    else if (action == ActionRequest::MoveForward) {
                        logger.logAction(tank->getPlayerId(), "Cancelling Move Back");
                        tank->setForward();
                        continue;
                    }
                    else {
                        continue;
                    }
                }
            }

            tank->setPrevPos();
            if (!isActionLegal(action, tank)) {
                logger.logAction(tank->getPlayerId(), actionToString(action) + " (ignored)");
                continue;
            }

            if(action != ActionRequest::MoveBackward) {
                logger.logAction(tank->getPlayerId(), actionToString(action));
                tank->setForward();
            }

            // Execute the action
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
                    tank->rotate(-90);
                    break;
                case ActionRequest::RotateRight90:
                    tank->rotate(90);
                    break;
                case ActionRequest::RotateLeft45:
                    tank->rotate(-45);
                    break;
                case ActionRequest::RotateRight45:
                    tank->rotate(45);
                    break;
                case ActionRequest::Shoot:
                    if (canTankShoot(tank)) {
                        shoot(tank);
                    }
                    break;
                case ActionRequest::DoNothing:
                    break;
                default:
                    break;
            }
        }
    }
}

void GameManager::logState() const {
    std::cout << "Game step: " << getGameStep() + 1 << "\n";
    board.printBoard();
}

Position GameManager::getNextPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd) {
    return board.wrap(elem->getNextPos(bkwd));
}

bool GameManager::canMove(std::shared_ptr<Tank> tank, bool bkwd) {
    Position next = getNextPosOnBoard(tank, bkwd);
    if(board.isWall(next)){
        if (bkwd) {
            tank->setForward();
            logger.logBadStep(tank->getPlayerId(), "Can't move back because wall in position " + next.toString());
        }
        else {
            logger.logBadStep(tank->getPlayerId(), "Can't move forward because wall in position " + next.toString());
        }
            return false;  // 🚧 Wall is in the way!

    }

    return true;  // ✅ No wall → we can move
}

void GameManager::move(std::shared_ptr<MovingElement> elem, bool bkwd){
    Position newPos = getNextPosOnBoard(elem, bkwd);
    board.removeSpecificObject(elem);  // move tank
    elem->setPrevPos();
    elem->setPosition(newPos);
    board.placeObject(elem);
}

// TODO: add collision type
bool GameManager::checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2){
    Position p1 = elem1->getPosition();
    Position p2 = elem2->getPosition();

    Position prev1 = elem1->getPrevPos();
    Position prev2 = elem2->getPrevPos();

    // If they swapped positions between last turn and now
    return (p1 == prev2 && p2 == prev1);
}

// TODO: add collision type
void GameManager::checkTankCollisions(std::shared_ptr<Tank> tank, std::unordered_set<GameObjectPtr>& marked) {
    if (!tank || tank->isDestroyed()) return;

    Position pos = tank->getPosition();
    const auto& objects = board.getObjectsAt(pos);

    for (const auto& obj : objects) {
        if (!obj || obj == tank || obj->isDestroyed()) continue;

        // Check tank-to-tank collision
        if (auto otherTank = std::dynamic_pointer_cast<Tank>(obj)) {
            if (otherTank->getPlayerId() != tank->getPlayerId()) {
                // Enemy tank collision - both tanks are destroyed
                tank->setCollisionType("enemy tank collision");
                otherTank->setCollisionType("enemy tank collision");
                marked.insert(tank);
                marked.insert(otherTank);
            }
            continue;
        }

        // Check tank-to-wall collision
        if (std::dynamic_pointer_cast<Wall>(obj)) {
            tank->setCollisionType("wall collision");
            marked.insert(tank);
            continue;
        }

        // Check tank-to-mine collision
        if (std::dynamic_pointer_cast<Mine>(obj)) {
            tank->setCollisionType("mine explosion");
            marked.insert(tank);
            continue;
        }
    }
}

// TODO: add collision type
void GameManager::checkShellCollisions(std::shared_ptr<Shell> shell, std::unordered_set<GameObjectPtr>& marked) {
    if (!shell) return;

    Position currPos = shell->getPosition();

    // 1. Check passing collisions with other shells
    for (const auto& other : shells) {
        if (!other || other == shell) continue;

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

// TODO: log the bad steps
bool GameManager::isActionLegal(ActionRequest act, std::shared_ptr<Tank> tank) {
    switch (act) {
        case ActionRequest::MoveForward: return canMove(tank, false);
        case ActionRequest::MoveBackward: return canMove(tank, true);
        case ActionRequest::Shoot: return canTankShoot(tank);
        default: return true;
    }
}

bool GameManager::canTankShoot(std::shared_ptr<Tank> tank){
    if (!tank->canShoot()){
        logger.logBadStep(tank->getPlayerId(), "Tried to shoot before cooldown ended.");
        return false;
    }
    return true;
}

bool GameManager::shoot(std::shared_ptr<Tank> tank) {
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
    return true;
}

void GameManager::destroyAndRemove(const GameObjectPtr& obj) {
    if (!obj) return;

    obj->destroy();  // calls GameObject's destroy logic

    if (obj->isDestroyed()) {
        board.removeSpecificObject(obj);  // ✅ remove only if marked destroyed
    }
}

std::unique_ptr<SatelliteView> GameManager::getSatelliteView(const Tank& tank) const {
    auto boardMat = board.getBoardMat();
    Position pos = tank.getPosition();

    if (pos.y < board.getHeight() && pos.x < board.getWidth()) {
        boardMat[pos.y][pos.x] = '%';
    }

    return std::make_unique<BoardSatelliteView>(std::move(boardMat));
}

bool GameManager::readBoard(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << inputFile << std::endl;
        return false;
    }

    std::string line;
    std::string mapName;
    int maxSteps = 0;
    int numShells = 0;
    int rows = 0;
    int cols = 0;

    // Read map name/description (Line 1)
    if (!std::getline(file, mapName)) {
        std::cerr << "Error: Empty file" << std::endl;
        return false;
    }

    // Helper function to remove spaces from a string
    auto removeSpaces = [](std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    };

    // Read MaxSteps (Line 2)
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing MaxSteps line" << std::endl;
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "MaxSteps=%d", &maxSteps) != 1) {
        std::cerr << "Error: Invalid MaxSteps format" << std::endl;
        return false;
    }

    // Read NumShells (Line 3)
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing NumShells line" << std::endl;
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%d", &numShells) != 1) {
        std::cerr << "Error: Invalid NumShells format" << std::endl;
        return false;
    }

    // Read Rows (Line 4)
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing Rows line" << std::endl;
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%d", &rows) != 1) {
        std::cerr << "Error: Invalid Rows format" << std::endl;
        return false;
    }

    // Read Cols (Line 5)
    if (!std::getline(file, line)) {
        std::cerr << "Error: Missing Cols line" << std::endl;
        return false;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%d", &cols) != 1) {
        std::cerr << "Error: Invalid Cols format" << std::endl;
        return false;
    }

    // Validate dimensions
    if (rows <= 0 || cols <= 0) {
        std::cerr << "Error: Invalid dimensions" << std::endl;
        return false;
    }

    // Initialize the board with the specified dimensions
    board = GameBoard(rows, cols);

    // Read the map content
    std::vector<std::string> mapLines;
    while (std::getline(file, line)) {
        mapLines.push_back(line);
    }

    // Process the map content
    std::map<int, bool> hasPlayerTank;
    std::map<int, int> playerTankCount;

    for (int row = 0; row < rows; row++) {
        std::string currentLine = (row < mapLines.size()) ? mapLines[row] : "";
        
        for (int col = 0; col < cols; col++) {
            char cell = (col < currentLine.length()) ? currentLine[col] : ' ';
            Position pos(row, col);

            switch (cell) {
                case '#': // Wall
                    board.addObject(std::make_shared<Wall>(pos));
                    break;
                case '@': // Mine
                    board.addObject(std::make_shared<Mine>(pos));
                    break;
                case ' ': // Empty space
                    break;
                default: // Tank or other character
                    if (cell >= '1' && cell <= '9') {  // Support up to 9 players
                        int playerId = cell - '0';
                        hasPlayerTank[playerId] = true;
                        playerTankCount[playerId]++;
                        
                        // Create tank with appropriate direction
                        Direction initialDir = (playerId == 1) ? Direction::LEFT : Direction::RIGHT;
                        auto tank = std::make_shared<Tank>(pos, initialDir, playerId, numShells);
                        
                        // Add to player's army
                        playersArmy[playerId].push_back(tank);
                        board.addObject(tank);
                        
                        // Create and add algorithm for this tank
                        auto algorithm = algorithmFactory.create(playerId, playersArmy[playerId].size() - 1);
                        tank->setAlgorithm(std::move(algorithm));
                    }
                    break;
            }
        }
    }

    // Create players for each player ID found
    players.clear();
    for (const auto& [playerId, hasTank] : hasPlayerTank) {
        if (hasTank) {
            players.push_back(playerFactory.create(playerId, cols, rows, maxSteps, numShells));
        }
    }

    // Check if we have at least two players with tanks
    if (players.size() < 2) {
        logger.logResult("Error: Need at least two players with tanks to start the game");
        return false;
    }

    // Set game parameters
    this->maxSteps = maxSteps;
    stalemateSteps = STALEMATE_STEPS;
    validGame = true;
    currentPlayerIndex = 0;

    return true;
}

int GameManager::getGameStep() const {
    return stepCount / 2;  // Each game step consists of two stepCount increments
}



