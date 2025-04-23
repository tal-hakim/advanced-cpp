#include <gtest/gtest.h>
#include "../include/GameState.h"
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Wall.h"
#include "../include/Mine.h"

class GameStateTest : public ::testing::Test {
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
        
        // Create walls
        wall1 = new Wall(Position(4, 4));
        wall2 = new Wall(Position(5, 5));
        board->placeObject(wall1);
        board->placeObject(wall2);
        
        // Create mine
        mine = new Mine(Position(3, 6));
        board->placeObject(mine);
    }

    void TearDown() override {
        delete board;  // Board doesn't own objects
        delete tank1;
        delete tank2;
        delete wall1;
        delete wall2;
        delete mine;
        
        // Delete shells
        for (Shell* shell : shells) {
            delete shell;
        }
        shells.clear();
    }

    Board* board;
    Tank* tank1;
    Tank* tank2;
    Wall* wall1;
    Wall* wall2;
    Mine* mine;
    std::vector<Shell*> shells;
};

TEST_F(GameStateTest, ConstructorAndGetters) {
    GameState state(board, tank1, tank2, shells);
    
    EXPECT_EQ(state.getBoard(), board);
    EXPECT_EQ(state.getPlayerTank(), tank1);
    EXPECT_EQ(state.getEnemyTank(), tank2);
    EXPECT_EQ(state.getShells().size(), 0);
}

TEST_F(GameStateTest, IsInLineOfSight_ClearPath) {
    // Position tank1 directly to the left of tank2 with no obstacles
    tank1->setPosition(Position(2, 7));
    tank1->setDirection(Direction::RIGHT);
    board->removeObject(Position(2, 2));
    board->placeObject(tank1);
    
    // Remove walls between them
    board->removeObject(wall1->getPosition());
    board->removeObject(wall2->getPosition());
    
    GameState state(board, tank1, tank2, shells);
    
    // Tank1 should be able to see tank2
    EXPECT_TRUE(state.isInLineOfSight(tank1, tank2));
    
    // Tank2 is not facing tank1 by default
    tank2->setDirection(Direction::UP); // Ensure it's not facing tank1
    EXPECT_FALSE(state.isInLineOfSight(tank2, tank1));
    
    // Change tank2's direction to face tank1
    tank2->setDirection(Direction::LEFT);
    EXPECT_TRUE(state.isInLineOfSight(tank2, tank1));
}

TEST_F(GameStateTest, IsInLineOfSight_Blocked) {
    // Position tank1 directly to the left of tank2
    tank1->setPosition(Position(2, 7));
    tank1->setDirection(Direction::RIGHT);
    board->removeObject(Position(2, 2));
    board->placeObject(tank1);
    
    // Place a wall between them
    Wall* blockingWall = new Wall(Position(4, 7));
    board->placeObject(blockingWall);
    
    GameState state(board, tank1, tank2, shells);
    
    // Wall should block line of sight
    EXPECT_FALSE(state.isInLineOfSight(tank1, tank2));
    
    // Clean up
    delete blockingWall;
}

TEST_F(GameStateTest, IsShellApproaching) {
    // Create a shell heading toward tank1
    Shell* shell = new Shell(Position(5, 2), Direction::LEFT, 2);
    shells.push_back(shell);
    board->placeObject(shell);
    
    GameState state(board, tank1, tank2, shells);
    
    // Shell is approaching tank1
    EXPECT_TRUE(state.isShellApproaching(tank1));
    
    // Shell is not approaching tank2
    EXPECT_FALSE(state.isShellApproaching(tank2));
    
    // Create a shell from tank1 - should not be considered as approaching tank1
    Shell* ownShell = new Shell(Position(3, 2), Direction::RIGHT, 1);
    shells.push_back(ownShell);
    board->placeObject(ownShell);
    
    // Own shell should not be considered as approaching
    EXPECT_TRUE(state.isShellApproaching(tank1)); // Only the enemy shell counts
}

TEST_F(GameStateTest, GetNearbyEmptyPositions) {
    GameState state(board, tank1, tank2, shells);
    
    // Get empty positions around tank1
    std::vector<Position> emptyPositions = state.getNearbyEmptyPositions(tank1->getPosition());
    
    // Should have at least some empty positions nearby
    EXPECT_FALSE(emptyPositions.empty());
    
    // Verify that none of the positions have objects
    for (const Position& pos : emptyPositions) {
        EXPECT_FALSE(board->isOccupied(pos));
    }
}

TEST_F(GameStateTest, IsSafePosition) {
    GameState state(board, tank1, tank2, shells);
    
    // Empty position should be safe
    EXPECT_TRUE(state.isSafePosition(Position(1, 1)));
    
    // Wall position should not be safe
    EXPECT_FALSE(state.isSafePosition(wall1->getPosition()));
    
    // Mine position should not be safe
    EXPECT_FALSE(state.isSafePosition(mine->getPosition()));
    
    // Tank position should not be safe
    EXPECT_FALSE(state.isSafePosition(tank1->getPosition()));
    EXPECT_FALSE(state.isSafePosition(tank2->getPosition()));
} 