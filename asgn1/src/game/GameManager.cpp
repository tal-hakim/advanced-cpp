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
    algo1 = std::make_unique<AlgorithmChaser>();
    algo2 = std::make_unique<AlgorithmEvader>();
}

bool GameManager::isPlayerTurn() {
    return stepCount % 2 == 0;
}

void GameManager::runGame() {
    while (stepCount < 1000) {
        std::vector<GameObjectPtr> markedForDestruction;
        moveShells();
        //TODO: check if collision
        if(isPlayerTurn()){
            executeTanksStep();
            //TODO: check if collision
        }
        for (const auto& obj : markedForDestruction) {
            if (obj) {
                obj->destroy(board);
            }
        }
        // TODO: check if game over

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

void GameManager::moveShells() {
    std::vector<std::shared_ptr<Shell>> updatedShells;

    for (auto& shell : shells) {
        if (shell->isDestroyed()) continue;

        move(shell, false);  // ✅ move forward one half-step
        updatedShells.push_back(shell);  // keep shell alive for next frame
    }

    shells = std::move(updatedShells);
}

void GameManager::updateTanks() {
    for (const auto& tank : tanks) {
        if (!tank->isDestroyed()) board.moveObj(tank);
    }
}

void GameManager::updateShells() {
    for (const auto& shell : shells) {
        if (!shell->isDestroyed()) board.moveObj(shell);
    }
}


void GameManager::executeTanksStep() {
    Action action1 = algo1->getNextAction(board, *tank1, *tank2);
    Action action2 = algo2->getNextAction(board, *tank2, *tank1);

    // 1. Handle tank actions
    for (size_t i = 0; i < tanks.size(); ++i) {
        auto tank = tanks[i];
        Action action = (tank->getPlayerId() == 1) ? action1 : action2;
        tank->decreaseShootCooldown();
        // TODO: update which bad step!!
        if (!isActionLegal(action, tank)) {
            logger.logBadStep(tank->getPlayerId(), "Bad Step");
            continue;
        }
        //TODO: Add logger stuff
        if (tank->isGoingBack()) {
            tank->decreaseBackwardTimer();
            if(!tank->isLastStepBack(stepCount) || action != Action::MoveBack) {
                if (tank->getBackwardTimer() == 0) {
                    action = Action::MoveBack;
                }
                else if (action == Action::MoveFwd) {
                    tank->setForward();
                    continue;
                }
                else {
                    continue;
                }
            }

        }

        if(action != Action::MoveBack) {
            tank->setForward();
        }

        switch (action) {
            case Action::MoveFwd: {
                move(tank, false);
                break;
            }
            case Action::MoveBack: {
                if(!tank->isGoingBack()) {
                    tank->setBackwards();
                }
                else {
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


void GameManager::checkCollisions() {

}


void GameManager::checkPassingCollision(std::shared_ptr<MovingElement> elem1, std::shared_ptr<MovingElement> elem2){
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
