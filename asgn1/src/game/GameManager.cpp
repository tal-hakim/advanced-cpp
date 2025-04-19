#include "game/GameManager.h"
#include "objects/Tank.h"
#include "objects/Wall.h"
#include "objects/Shell.h"
#include "objects/Mine.h"
#include "game/Action.h"
#include "definitions.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

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
                case '1':
                    tank1 = std::make_shared<Tank>(pos, Direction::L, 1);
                    board.placeObject(tank1);
                    break;
                case '2':
                    tank2 = std::make_shared<Tank>(pos, Direction::R, 2);
                    board.placeObject(tank2);
                    break;
                default:
                    break;
            }
        }
        ++y;
    }

    // Plug in your two algorithms
    algo1 = std::make_unique<AlgorithmChaser>();
    algo2 = std::make_unique<AlgorithmEvader>();
}

void GameManager::runGame() {
    while (stepCount < 1000) {
        for shell in shells : move
        check if collision
        if(stepCount % 2 == 0){
            executeStep();
        }
        ++stepCount;

        // Example condition: both tanks still alive?
        if (!tank1 || !tank2)
            break;
    }

    if (!tank1 && !tank2)
        logger.logResult("Tie - Both tanks destroyed");
    else if (!tank1)
        logger.logResult("Player 2 wins - Player 1 destroyed");
    else if (!tank2)
        logger.logResult("Player 1 wins - Player 2 destroyed");
    else
        logger.logResult("Tie - Step limit reached");

    logger.finalize();
}

void GameManager::executeStep() {
    Action action1 = algo1->getNextAction(board, *tank1, *tank2);
    Action action2 = algo2->getNextAction(board, *tank2, *tank1);
    std::vector<std::shared_ptr<Tank>> tanks = {tank1, tank2};

    // 1. Handle tank actions
    for (size_t i = 0; i < tanks.size(); ++i) {
        auto tank = tanks[i];
        Action action = (i == 0) ? action1 : action2;

        if (!isActionLegal(action, tank)) {
            logger.logBadStep(tank->getPlayerId(), "Bad Step");
            continue;
        }

        if ( tank->getBackwardTimer() == 0 && tank->isGoingBack()) {
            move(tank, true);
            tank->resetBackwardTimer();
            continue;
        }

        switch (action) {
            case Action::MoveFwd: {
                if(!tank->getBackwardTimer() < )
                moveTank(tank, false);
                break;
            }
            case Action::MoveBack: {
                if()
                moveTank(tank, true);
                break;
            }
            case Action::Shoot: {
                shoot(tank);
                break;
            }
            case Action::RotateLeft_1_8:
            case Action::RotateRight_1_8:
            case Action::RotateLeft_1_4:
            case Action::RotateRight_1_4: {
                tank->rotate(static_cast<int>(action));
                break;
            }
            case Action::None:
            default:
                break;
        }
    }

    // 2. Move all shells
    updateShells();

    // 3. Update tank cooldowns
    for (auto& tank : tanks) {
        tank->tickCooldown();  // assuming this reduces shootCooldown each turn
    }

    // 4. Check victory condition (optional)
    checkGameOver();
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
    auto target = board.getObjectAt(next);
    if (std::shared_ptr<Wall> wall = std::dynamic_pointer_cast<Wall>(target)) {
        return false;
    }
    return true;
}

void GameManager::move(std::shared_ptr<MovingElement> elem, bool bkwd){
    Position newPos = getPosOnBoard(elem, bkwd);
    board.removeObjectAt(elem->getPosition());  // move tank
    elem->setPrevPos();
    elem->setPosition(newPos);
    board.placeObject(elem);
}

void GameManager::checkMovingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2){
    if (!elem1 || !elem2) return;

    Position p1 = elem1->getPosition();
    Position p2 = elem2->getPosition();

    Position prev1 = elem1->getPrevPos();
    Position prev2 = elem2->getPrevPos();

    // If they swapped positions between last turn and now
    if (p1 == prev2 && p2 == prev1) {
        logger.logBadStep("Moving collision detected between two elements at (" +
                   std::to_string(p1.x) + ", " + std::to_string(p1.y) + ")");

        // Remove both from the board
        board.removeObjectAt(p1);
        board.removeObjectAt(p2);

        // Optionally: mark them as destroyed if needed
        // elem1->destroy();
        // elem2->destroy();
    }
}
}

void GameManager::moveFwd(std::shared_ptr<Tank> tank){
    move(tank, false);
    tank->setForward();
}

bool GameManager::moveBkwd(std::shared_ptr<Tank> tank){
    move(tank, true);
    tank->setBackwards();
    tank->setBackwardTimer(BACKWARDS_STEP_COUNT);
}

bool GameManager::isActionLegal(Action act, std::shared_ptr<Tank> tank) {
    if(tank->getBackwardTimer() > 0 && tank->isGoingBack() && act != Action::MoveFwd){
        return false;
    }
    switch (act) {
        case Action::MoveFwd: return canMove(tank, false);
        case Action::MoveBack: return canMove(tank, true);
        case Action::Shoot: return tank->canShoot();
        default: return true;
    }
}

void GameManager::doAction(Action act, std::shared_ptr<Tank> tank) {
    bool legalStep;
    switch (act) {
        case Action::MoveFwd: legalStep=moveFwd(tank); break;
        case Action::MoveBack: legalStep=moveBkwd(tank); break;
        case Action::RotateLeft_1_4: tank->rotate(LEFT_ANGLE_1_4);
            break;
        case Action::RotateRight_1_4: tank->rotate(RIGHT_ANGLE_1_4); break;
        case Action::RotateLeft_1_8: tank->rotate(LEFT_ANGLE_1_8); break;
        case Action::RotateRight_1_8: tank->rotate(RIGHT_ANGLE_1_8); break;
        case Action::Shoot: legalStep=shoot(tank);break;
        default:break;
    }
    tank->setPrevAction(act);
}

bool GameManager::shoot(std::shared_ptr<Tank> tank) {
    if (!tank) return false;

    if (!tank->canShoot()) {
        logger.logBadStep(tank->getPlayerId(), "Tried to shoot but cannot (cooldown or out of shells)");
        return false;
    }

    // Create shell
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
