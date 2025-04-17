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
        executeStep();
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

    // TODO: validate and apply actions here
    // Examples:
    // - moveForward() if allowed
    // - shoot() if not in cooldown
    // - rotate if allowed
    // - log invalid actions with logger.logBadStep()

    logger.logStep(stepCount, "Action1", "Action2");
}

void GameManager::logState() const {
    std::cout << "Game step: " << stepCount << "\n";
    board.printBoard();
}

void GameManager::doAction(Action act, std::shared_ptr<Tank> tank) {
    switch (act) {
        case Action::MoveFwd: {Position newPos = board.wrap(tank->getNextPos());
        board.removeObjectAt(tank->getPosition());  // move tank
        tank->setPosition(newPos);
        board.placeObject(tank);
        tank->setForward();
        break;}
        case Action::MoveBack: {Position newPos = board.wrap(tank->getNextPos(true));
            board.removeObjectAt(tank->getPosition());  // move tank
            tank->setPosition(newPos);
            board.placeObject(tank);
            tank->setBackwards();
            tank->setBackwardTimer(BACKWARDS_STEP_COUNT);
            break;}
        case Action::RotateLeft_1_4: tank->rotate(LEFT_ANGLE_1_4);
            break;
        case Action::RotateRight_1_4: tank->rotate(RIGHT_ANGLE_1_4); break;
        case Action::RotateLeft_1_8: tank->rotate(LEFT_ANGLE_1_8); break;
        case Action::RotateRight_1_8: tank->rotate(RIGHT_ANGLE_1_8); break;
        case Action::Shoot: {
            Shell 
        }


    }
}
