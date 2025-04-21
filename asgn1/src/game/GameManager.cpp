#include "game/GameManager.h"

GameManager::GameManager(const std::string& inputFile)
        : board(20, 20), logger(inputFile), stepCount(0) // default size, replace with parsed size
{
    std::ifstream in(inputFile);
    if (!in) {
        std::cerr << "Cannot open input file: " << inputFile << std::endl;
        std::exit(1);
    }

    int width = 0, height = 0;
    in >> width >> height;
    board = GameBoard(width, height);

    std::string line;
    std::getline(in, line); // consume leftover newline

    int y = 0;
    while (std::getline(in, line) && y < height) {
        for (int x = 0; x < std::min(width, (int)line.length()); ++x) {
            char c = line[x];
            Position pos(x, y);

            switch (c) {
                case '#':
                    board.placeObject(std::make_shared<Wall>(pos));
                    break;
                case '@':
                    board.placeObject(std::make_shared<Mine>(pos));
                    break;
                case '1': {
                    //tank1 = std::make_shared<Tank>(pos, Direction::L, 1);
                    auto player_1_tank = std::make_shared<Tank>(pos, Direction::L, PLAYER_1_ID);
                    tanks.push_back(player_1_tank);
                    board.placeObject(player_1_tank);
                    break;
                }
                case '2': {
                    auto player_2_tank = std::make_shared<Tank>(pos, Direction::R, PLAYER_2_ID);
                    tanks.push_back(player_2_tank);
                    board.placeObject(player_2_tank);
                    break;
                }
                default:
                    break;
            }
        }
        ++y;
    }
    // Plug in your two algorithms
    algo1 = std::make_unique<Chaser>();
    algo2 = std::make_unique<Evader>();
}

std::string GameManager::actionToString(Action action) const {
    switch (action) {
        case Action::MoveFwd: return "Move Forward";
        case Action::MoveBack: return "Move Backwards";
        case Action::Shoot: return "Shoot";
        case Action::RotateLeft_1_8: return "RotateLeft 1/8";
        case Action::RotateRight_1_8: return "RotateRight 1/8";
        case Action::RotateLeft_1_4: return "RotateLeft 1/4";
        case Action::RotateRight_1_4: return "RotateRight 1/4";
        case Action::None: return "None";
        default: return "Unknown";
    }
}


bool GameManager::isPlayerTurn() const {
    return stepCount % 2 == 0;
}


bool GameManager::isGameOver() const {
    if (stepsRemaining <= 0) {
        logger.logResult("Tie - no ammo for the last 40 steps");
        return true;
    }

    std::shared_ptr<Tank> tank1 = nullptr;
    std::shared_ptr<Tank> tank2 = nullptr;

    for (const auto& tank : tanks) {
        if (!tank) continue;
        if (tank->getPlayerId() == PLAYER_1_ID) tank1 = tank;
        if (tank->getPlayerId() == PLAYER_2_ID) tank2 = tank;
    }

    bool tank1Dead = tank1->isDestroyed();
    bool tank2Dead = tank2->isDestroyed();

    if (tank1Dead && tank2Dead) {
        logger.logResult(std::string("Tie - Both tanks destroyed\n") +
        "Player 1: tank destroyed by " + tank1->getCollisionType() + "\n" +
        "Player 2: tank destroyed by " + tank2->getCollisionType()
        );
        return true;
    }

    if (tank1Dead) {
        logger.logResult("Player 2 wins - Player 1 tank destroyed by " + tank1->getCollisionType());
        return true;
    }

    if (tank2Dead) {
        logger.logResult("Player 1 wins - Player 2 tank destroyed by " + tank2->getCollisionType());
        return true;
    }

    return false;  // game continues
}



void GameManager::runGame() {

    while (!isGameOver()) {
        std::unordered_set<GameObjectPtr> markedForDestruction;
        moveShells();
        // check shells collisions
        for (const auto& shell : shells) {
            checkShellCollisions(shell, markedForDestruction);
        }
        if(isPlayerTurn()){
            if (areAllTanksOutOfAmmo()) stepsRemaining--;
            executeTanksStep();
            // check tanks collisions
            for (const auto& tank : tanks) {
                checkTankCollisions(tank, markedForDestruction);
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
    for (const auto& tank : tanks) {
        if (tank && !tank->isOutOfAmmo()) {
            return false;  // At least one tank has ammo
        }
    }
    return true;  // ✅ All tanks are out of ammo
}

void GameManager::moveShells() {
    std::vector<std::shared_ptr<Shell>> updatedShells;

    for (auto& shell : shells) {
        if (shell->isDestroyed()) continue;

        move(shell, false);  // ✅ move forward one half-step
        updatedShells.push_back(shell);  // keep shell alive for next frame
    }

    shells = std::move(updatedShells);
}


void GameManager::executeTanksStep() {
    Action action1 = algo1->getNextAction(board, *tank1, *tank2);
    Action action2 = algo2->getNextAction(board, *tank2, *tank1);

    // 1. Handle tank actions
    for (size_t i = 0; i < tanks.size(); ++i) {
        auto tank = tanks[i];
        Action action = (tank->getPlayerId() == 1) ? action1 : action2;
        tank->decreaseShootCooldown();

        if (!isActionLegal(action, tank)) {
            continue;
        }

        if (tank->isGoingBack()) {
            tank->decreaseBackwardTimer();
            if(!tank->isLastStepBack(stepCount) || action != Action::MoveBack) {
                if (tank->getBackwardTimer() == 0) {
                    action = Action::MoveBack;
                }
                else if (action == Action::MoveFwd) {
                    logger.logStep(stepCount, tank->getPlayerId(), "Cancelling Move Back");
                    tank->setForward();
                    continue;
                }
                else {
                    continue;
                }
            }

        }

        if(action != Action::MoveBack) {
            logger.logStep(stepCount, tank->getPlayerId(), actionToString(action));
            tank->setForward();
        }

        switch (action) {
            case Action::MoveFwd: {
                move(tank, false);
                break;
            }
            case Action::MoveBack: {
                if(!tank->isGoingBack()) {
                    logger.logStep(stepCount, tank->getPlayerId(), "Will move back in 3 steps");
                    tank->setBackwards();
                }
                else {
                    logger.logStep(stepCount, tank->getPlayerId(), actionToString(action));
                    move(tank, true);
                    tank->setLastBackwardStep(stepCount);
                }
                break;
            }
            case Action::Shoot: {
                shoot(tank);
                break;
            }
            case Action::RotateLeft_1_8: tank->rotate(LEFT_ANGLE_1_8); break;
            case Action::RotateRight_1_8: tank->rotate(RIGHT_ANGLE_1_8); break;
            case Action::RotateLeft_1_4: tank->rotate(LEFT_ANGLE_1_4); break;
            case Action::RotateRight_1_4: tank->rotate(RIGHT_ANGLE_1_4); break;
            default:
                break;
        }
    }
}

void GameManager::logState() const {
    std::cout << "Game step: " << stepCount << "\n";
    board.printBoard();
}

Position GameManager::getPosOnBoard(std::shared_ptr<MovingElement> elem, bool bkwd) {
    return board.wrap(elem->getNextPos(bkwd));
}

bool GameManager::canMove(std::shared_ptr<MovingElement> elem, bool bkwd) {
    Position next = getPosOnBoard(elem, bkwd);
    auto objects = board.getObjectsAt(next);

    for (const auto& obj : objects) {
        if (std::dynamic_pointer_cast<Wall>(obj)) {
            if (bkwd) {
                logger.logBadStep(elem->getPlayerId(), "Can't move back because wall in position " + obj->getPosition().toString());

            }
            else {
                logger.logBadStep(elem->getPlayerId(), "Can't move forward because wall in position " + obj->getPosition().toString());
            }
            return false;  // 🚧 Wall is in the way!
        }
    }

    return true;  // ✅ No wall → we can move
}


void GameManager::move(std::shared_ptr<MovingElement> elem, bool bkwd){
    Position newPos = getPosOnBoard(elem, bkwd);
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
    if (!tank) return;

    Position currPos = tank->getPosition();

    // 1. Passing collision with other tanks
    for (const auto& other : tanks) {
        if (!other || other == tank) continue;

        if (checkPassingCollision(tank, other)) {
            tank->setCollisionType(other->toString());
            other->setCollisionType(tank->toString());
            marked.insert(tank);
            marked.insert(other);
        }
    }

    // 2. Passing collision with shells
    for (const auto& shell : shells) {
        if (!shell) continue;

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
bool GameManager::isActionLegal(Action act, std::shared_ptr<Tank> tank) {
    switch (act) {
        case Action::MoveFwd: return canMove(tank, false);
        case Action::MoveBack: return canMove(tank, true);
        case Action::Shoot: return canTankShoot(tank);
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

    // Optionally: place it on the board (if you want it visible on the grid)
    board.placeObject(shell);

    tank->shoot();
    return true;
}

void GameManager::destroyAndRemove(const GameObjectPtr& obj) {
    if (!obj) return;

    obj->destroy(board);  // calls GameObject's destroy logic

    if (obj->isDestroyed()) {
        board.removeSpecificObject(obj);  // ✅ remove only if marked destroyed
    }
}

