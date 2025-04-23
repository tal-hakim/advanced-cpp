#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/SimpleAlgorithm.h"
#include "../include/PathfindingAlgorithm.h"
#include "../include/GameState.h"
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Wall.h"
#include "../include/Mine.h"

class AlgorithmsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a 10x10 board
        board = new Board(10, 10);
        
        // Create tanks
        tank1 = new Tank(Position(2, 2), 1);
        tank2 = new Tank(Position(7, 7), 2);
        
        // Place tanks on board
        board->placeObject(tank1);
        board->placeObject(tank2);
        
        // Create algorithms
        simpleAlgo = new SimpleAlgorithm();
        pathAlgo = new PathfindingAlgorithm();
    }

    void TearDown() override {
        delete board;  // Board doesn't own objects
        delete tank1;
        delete tank2;
        
        // Delete algorithms
        delete simpleAlgo;
        delete pathAlgo;
        
        // Delete shells
        for (Shell* shell : shells) {
            delete shell;
        }
        shells.clear();
        
        // Delete any walls or mines
        for (auto obj : objects) {
            delete obj;
        }
        objects.clear();
    }
    
    // Helper to add a wall
    Wall* addWall(int x, int y) {
        Wall* wall = new Wall(Position(x, y));
        board->placeObject(wall);
        objects.push_back(wall);
        return wall;
    }
    
    // Helper to add a mine
    Mine* addMine(int x, int y) {
        Mine* mine = new Mine(Position(x, y));
        board->placeObject(mine);
        objects.push_back(mine);
        return mine;
    }
    
    // Helper to add a shell
    Shell* addShell(int x, int y, Direction dir, int ownerId) {
        Shell* shell = new Shell(Position(x, y), dir, ownerId);
        board->placeObject(shell);
        shells.push_back(shell);
        return shell;
    }

    Board* board;
    Tank* tank1;
    Tank* tank2;
    SimpleAlgorithm* simpleAlgo;
    PathfindingAlgorithm* pathAlgo;
    std::vector<Shell*> shells;
    std::vector<GameObject*> objects;
};

TEST_F(AlgorithmsTest, SimpleAlgorithm_GetName) {
    EXPECT_EQ(simpleAlgo->getName(), "SimpleAlgorithm");
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_GetName) {
    EXPECT_EQ(pathAlgo->getName(), "PathfindingAlgorithm");
}

TEST_F(AlgorithmsTest, SimpleAlgorithm_ShootWhenInLineOfSight) {
    // Position tanks in line of sight
    tank1->setPosition(Position(2, 5));
    tank1->setDirection(Direction::RIGHT);
    board->removeObject(Position(2, 2));
    board->placeObject(tank1);
    
    tank2->setPosition(Position(7, 5));
    board->removeObject(Position(7, 7));
    board->placeObject(tank2);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should decide to shoot when in line of sight
    Action action = simpleAlgo->decideAction(state);
    EXPECT_EQ(action, Action::SHOOT);
}

TEST_F(AlgorithmsTest, SimpleAlgorithm_EvadeApproachingShell) {
    // Add a shell approaching tank1
    addShell(5, 2, Direction::LEFT, 2);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should decide to evade
    Action action = simpleAlgo->decideAction(state);
    EXPECT_NE(action, Action::NONE);
    EXPECT_NE(action, Action::SHOOT);  // Shouldn't shoot when evading
}

TEST_F(AlgorithmsTest, SimpleAlgorithm_RotateToFaceEnemy) {
    // Tank is not in line of sight with enemy
    tank1->setDirection(Direction::UP);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should decide to rotate to face enemy
    Action action = simpleAlgo->decideAction(state);
    
    // Should be some type of rotation
    EXPECT_TRUE(action == Action::ROTATE_LEFT_EIGHTH || 
                action == Action::ROTATE_RIGHT_EIGHTH || 
                action == Action::ROTATE_LEFT_QUARTER || 
                action == Action::ROTATE_RIGHT_QUARTER);
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_FindPathToEnemy) {
    // Create a simple maze with walls
    addWall(3, 2);
    addWall(3, 3);
    addWall(3, 4);
    addWall(4, 4);
    addWall(5, 4);
    addWall(6, 4);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should find a path around walls
    Action action = pathAlgo->decideAction(state);
    
    // Should either rotate or move forward to start navigating
    EXPECT_TRUE(action == Action::MOVE_FORWARD || 
                action == Action::ROTATE_LEFT_EIGHTH || 
                action == Action::ROTATE_RIGHT_EIGHTH || 
                action == Action::ROTATE_LEFT_QUARTER || 
                action == Action::ROTATE_RIGHT_QUARTER);
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_ShootWhenInLineOfSight) {
    // Position tanks in line of sight
    tank1->setPosition(Position(2, 5));
    tank1->setDirection(Direction::RIGHT);
    board->removeObject(Position(2, 2));
    board->placeObject(tank1);
    
    tank2->setPosition(Position(7, 5));
    board->removeObject(Position(7, 7));
    board->placeObject(tank2);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should decide to shoot when in line of sight
    Action action = pathAlgo->decideAction(state);
    EXPECT_EQ(action, Action::SHOOT);
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_EvadeApproachingShell) {
    // Add a shell approaching tank1
    addShell(5, 2, Direction::LEFT, 2);
    
    // Create game state from tank1's perspective
    GameState state(board, tank1, tank2, shells);
    
    // Algorithm should decide to evade
    Action action = pathAlgo->decideAction(state);
    EXPECT_NE(action, Action::NONE);
}

TEST_F(AlgorithmsTest, BothAlgorithms_NoneWhenNoTank) {
    // Test with destroyed tanks
    tank1->destroy();
    tank2->destroy();
    
    GameState state(board, tank1, tank2, shells);
    
    // Both algorithms should return NONE for destroyed tanks
    EXPECT_EQ(simpleAlgo->decideAction(state), Action::NONE);
    EXPECT_EQ(pathAlgo->decideAction(state), Action::NONE);
} 