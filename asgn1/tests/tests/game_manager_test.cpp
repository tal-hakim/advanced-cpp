#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/GameManager.h"
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Wall.h"
#include "../include/Mine.h"
#include "../include/SimpleAlgorithm.h"

// Mock Algorithm for testing
class MockAlgorithm : public Algorithm {
public:
    MockAlgorithm(Action defaultAction) : defaultAction(defaultAction) {}
    
    Action decideAction(const GameState& state) override {
        return defaultAction;
    }
    
    std::string getName() const override {
        return "MockAlgorithm";
    }
    
    void setAction(Action action) {
        defaultAction = action;
    }
    
private:
    Action defaultAction;
};

class GameManagerTest : public ::testing::Test {
protected:
    Board* board;
    Tank* tank1;
    Tank* tank2;
    GameManager* gameManager;
    MockAlgorithm* mockAlgo1; // Don't delete, owned by gameManager
    MockAlgorithm* mockAlgo2; // Don't delete, owned by gameManager
    std::vector<GameObject*> objects;

    void SetUp() override {
        // Create a 10x10 board
        board = new Board(10, 10);
        
        // Create tanks
        tank1 = new Tank(Position(2, 2), 1);
        tank2 = new Tank(Position(7, 7), 2);
        
        // Place tanks on board
        board->placeObject(tank1);
        board->placeObject(tank2);
        
        // Create basic game manager
        gameManager = new GameManager(board, tank1, tank2);
        
        // Create mock algorithms that do nothing by default
        mockAlgo1 = new MockAlgorithm(Action::NONE);
        mockAlgo2 = new MockAlgorithm(Action::NONE);
        
        // Set algorithms
        gameManager->setAlgorithm1(mockAlgo1);
        gameManager->setAlgorithm2(mockAlgo2);
    }

    void TearDown() override {
        // Delete game manager (will delete algorithms)
        delete gameManager;
        
        // gameManager doesn't own these, so we need to delete them
        delete board;
        delete tank1;
        delete tank2;
        
        // Delete any created walls and mines
        for (auto obj : objects) {
            delete obj;
        }
    }
    
    // Helper to add a wall and track it
    Wall* addWall(int x, int y) {
        Wall* wall = new Wall(Position(x, y));
        board->placeObject(wall);
        objects.push_back(wall);
        return wall;
    }
    
    // Helper to add a mine and track it
    Mine* addMine(int x, int y) {
        Mine* mine = new Mine(Position(x, y));
        board->placeObject(mine);
        objects.push_back(mine);
        gameManager->registerMines({mine});
        return mine;
    }


};

TEST_F(GameManagerTest, InitialState) {
    EXPECT_FALSE(gameManager->isGameOver());
    EXPECT_EQ(gameManager->getStepHistory().size(), 0);
}

TEST_F(GameManagerTest, ProcessStep_NoAction) {
    // Process one step with default NONE actions
    gameManager->processStep();
    
    // Should have logged the steps
    EXPECT_EQ(gameManager->getStepHistory().size(), 2); // One log for each tank
    
    // Should not be game over
    EXPECT_FALSE(gameManager->isGameOver());
}

TEST_F(GameManagerTest, TankMovement) {
    // Set tank1 to move forward
    mockAlgo1->setAction(Action::MOVE_FORWARD);
    
    // Get initial position
    Position initialPos = tank1->getPosition();
    
    // Process step
    gameManager->processStep();
    
    // Tank should have moved
    EXPECT_NE(tank1->getPosition(), initialPos);
}

TEST_F(GameManagerTest, TankRotation) {
    // Set initial direction
    Direction initialDir = Direction::UP;
    tank1->setDirection(initialDir);
    
    // Set tank1 to rotate
    mockAlgo1->setAction(Action::ROTATE_LEFT_QUARTER);
    
    // Process step
    gameManager->processStep();
    
    // Direction should have changed
    EXPECT_NE(tank1->getDirection(), initialDir);
}

TEST_F(GameManagerTest, ShellCreation) {
    // Print initial position and direction (for debugging)
    std::cout << "Initial tank position: (" << tank1->getPosition().x << "," 
              << tank1->getPosition().y << "), Direction: " 
              << static_cast<int>(tank1->getDirection()) << std::endl;
    
    // Set initial direction for tank1 (to make shell position predictable)
    Direction initialDir = Direction::RIGHT;
    tank1->setDirection(initialDir);
    
    // Print updated direction (for debugging)
    std::cout << "Updated direction: " << static_cast<int>(initialDir) << std::endl;
    
    // Set tank1 to shoot
    mockAlgo1->setAction(Action::SHOOT);
    
    // Process step
    gameManager->processStep();
    
    // Tank ammo should be decreased
    EXPECT_EQ(tank1->getAmmunition(), 15); // Started with 16
    
    // Tank should have cooldown (initially set to 4, but decremented at the end of processTankAction)
    EXPECT_EQ(tank1->getCooldown(), 3);
    
    // There should be a shell on the board somewhere
    Position tankPos = tank1->getPosition();
    std::cout << "Tank position after step: (" << tankPos.x << "," << tankPos.y << ")" << std::endl;
    
    // Create shell directly to test
    Position shellPos(tankPos.x + 1, tankPos.y); // Tank is facing right, so shell is to the right
    Shell* testShell = new Shell(shellPos, initialDir, tank1->getId());
    board->placeObject(testShell);
    
    // Now verify that shell can be placed on the board
    GameObject* testObj = board->getObjectAt(shellPos);
    ASSERT_NE(testObj, nullptr) << "Test shell could not be placed at (" 
                                << shellPos.x << "," << shellPos.y << ")";
    
    // Clean up test shell to avoid memory leak
    board->removeObject(shellPos);
    delete testShell;
        
    // For testing, directly inspect all board positions to find any shells
    bool foundShell = false;
    Position foundShellPos(-1, -1);
    
    for (int y = 0; y < board->getHeight(); ++y) {
        for (int x = 0; x < board->getWidth(); ++x) {
            Position pos(x, y);
            GameObject* obj = board->getObjectAt(pos);
            if (obj && dynamic_cast<Shell*>(obj)) {
                foundShell = true;
                foundShellPos = pos;
                break;
            }
        }
        if (foundShell) break;
    }
    
    ASSERT_TRUE(foundShell) << "No shell found anywhere on the board";
    std::cout << "Found shell at position: (" << foundShellPos.x << "," 
              << foundShellPos.y << ")" << std::endl;
    
    // Use the found shell position for verification
    GameObject* obj = board->getObjectAt(foundShellPos);
    ASSERT_NE(obj, nullptr);
    
    Shell* shell = dynamic_cast<Shell*>(obj);
    ASSERT_NE(shell, nullptr);
    
    // Verify shell properties
    EXPECT_EQ(shell->getOwnerId(), tank1->getId());
    EXPECT_EQ(shell->getDirection(), initialDir);
}

TEST_F(GameManagerTest, TankMineCollision) {
    // Place a mine in front of tank1
    Direction dir = tank1->getDirection();
    Position pos = tank1->getPosition();
    Position delta = getDirectionDelta(dir);
    Position minePos(pos.x + delta.x, pos.y + delta.y);
    
    Mine* mine = addMine(minePos.x, minePos.y);
    
    // Set tank1 to move forward onto the mine
    mockAlgo1->setAction(Action::MOVE_FORWARD);
    
    // Process step
    gameManager->processStep();
    
    // Tank should be destroyed
    EXPECT_TRUE(tank1->isDestroyed());
    
    // Mine should be destroyed
    EXPECT_TRUE(mine->isDestroyed());
    
    // Game should be over, with player 2 winning
    EXPECT_TRUE(gameManager->isGameOver());
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("Player 2 wins"));
}

TEST_F(GameManagerTest, TankTankCollision) {
    // Move tanks next to each other
    tank1->setPosition(Position(4, 4));
    board->removeObject(Position(2, 2));
    board->placeObject(tank1);
    
    tank2->setPosition(Position(5, 4));
    board->removeObject(Position(7, 7));
    board->placeObject(tank2);
    
    // Set tank1 to move right into tank2
    tank1->setDirection(Direction::RIGHT);
    mockAlgo1->setAction(Action::MOVE_FORWARD);
    
    // Process step
    gameManager->processStep();
    
    // Both tanks should still be at their original positions (move blocked)
    EXPECT_EQ(tank1->getPosition(), Position(4, 4));
    EXPECT_EQ(tank2->getPosition(), Position(5, 4));
    
    // Neither tank should be destroyed
    EXPECT_FALSE(tank1->isDestroyed());
    EXPECT_FALSE(tank2->isDestroyed());
    
    // The step should be logged as a bad step
    EXPECT_THAT(gameManager->getStepHistory()[0], ::testing::HasSubstr("[BAD STEP]"));
}

TEST_F(GameManagerTest, ShellWallCollision) {
    // Set tank1's direction to make the test predictable
    tank1->setDirection(Direction::RIGHT);
    Direction dir = tank1->getDirection();
    Position pos = tank1->getPosition();
    Position delta = getDirectionDelta(dir);
    
    // Print initial positions (for debugging)
    std::cout << "Tank1 position: (" << pos.x << "," << pos.y << "), Direction: " 
              << static_cast<int>(dir) << std::endl;
    
    // Place wall 3 spaces ahead (to ensure the shell hits it during movement)
    Position wallPos(pos.x + 3*delta.x, pos.y + 3*delta.y);
    std::cout << "Wall position: (" << wallPos.x << "," << wallPos.y << ")" << std::endl;
    
    // Make sure the test area is clear
    // Remove any objects at the wall position
    if (board->isOccupied(wallPos)) {
        GameObject* obj = board->getObjectAt(wallPos);
        board->removeObject(wallPos);
        delete obj;
    }
    
    // Add the wall
    Wall* wall = addWall(wallPos.x, wallPos.y);
    ASSERT_NE(wall, nullptr);
    ASSERT_TRUE(board->isOccupied(wallPos));
    gameManager->registerWalls({wall});
    
    // Set tank1 to shoot
    mockAlgo1->setAction(Action::SHOOT);
    
    // Process step (shell created)
    gameManager->processStep();
    
    // Check shell creation
    Position shellPos(pos.x + delta.x, pos.y + delta.y);
    std::cout << "Expected initial shell position: (" << shellPos.x << "," << shellPos.y << ")" << std::endl;
    
    // Process another step (shell moves and hits wall)
    mockAlgo1->setAction(Action::NONE);  // Don't shoot again on this step
    gameManager->processStep();
    
    // Wall should be hit but not destroyed yet
    EXPECT_EQ(wall->getHitCount(), 1);
    EXPECT_FALSE(wall->isDestroyed());
    
    // Shell should be destroyed on collision with wall
    bool foundShell = false;
    for (int y = 0; y < board->getHeight(); ++y) {
        for (int x = 0; x < board->getWidth(); ++x) {
            Position checkPos(x, y);
            GameObject* obj = board->getObjectAt(checkPos);
            if (obj && dynamic_cast<Shell*>(obj)) {
                foundShell = true;
                break;
            }
        }
        if (foundShell) break;
    }
    EXPECT_FALSE(foundShell) << "Shell should be destroyed after hitting wall";
    
    // Wall should still be on the board
    EXPECT_TRUE(board->isOccupied(wallPos)) << "Wall should still be on the board after first hit";
    
    // Reset cooldown for tank1 so it can shoot again
    tank1->setCooldown(0);
    
    // Set tank1 to shoot again
    mockAlgo1->setAction(Action::SHOOT);
    
    // Process step (new shell created)
    gameManager->processStep();
    
    // Process another step (shell moves and hits wall again)
    mockAlgo1->setAction(Action::NONE);
    gameManager->processStep();
    
    // Wall should now be destroyed
    EXPECT_EQ(wall->getHitCount(), 2);
    EXPECT_TRUE(wall->isDestroyed());
    
    // Wall should be removed from the board after being destroyed
    EXPECT_FALSE(board->isOccupied(wallPos)) << "Wall should be removed from board after destruction";
}

TEST_F(GameManagerTest, ShellTankCollision) {
    // Move tank2 in front of tank1
    Direction dir = tank1->getDirection();
    Position pos = tank1->getPosition();
    Position delta = getDirectionDelta(dir);
    Position tank2Pos(pos.x + 3*delta.x, pos.y + 3*delta.y); // Three spaces ahead
    
    tank2->setPosition(tank2Pos);
    board->removeObject(Position(7, 7));
    board->placeObject(tank2);
    
    // Set tank1 to shoot
    mockAlgo1->setAction(Action::SHOOT);
    
    // Process steps until shell hits tank2
    gameManager->processStep(); // Shell created
    gameManager->processStep(); // Shell moves (2 spaces) and hits tank2
    
    // Tank2 should be destroyed
    EXPECT_TRUE(tank2->isDestroyed());
    
    // Game should be over, with player 1 winning
    EXPECT_TRUE(gameManager->isGameOver());
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("Player 1 wins"));
}

TEST_F(GameManagerTest, OutOfAmmoTie) {
    // Set both tanks to have no ammo
    tank1->setCooldown(0);
    while (tank1->getAmmunition() > 0) {
        tank1->decrementAmmunition();
    }
    
    tank2->setCooldown(0);
    while (tank2->getAmmunition() > 0) {
        tank2->decrementAmmunition();
    }
    
    // Process 40 steps
    for (int i = 0; i < 40; ++i) {
        gameManager->processStep();
    }
    
    // Game should be over with a tie
    EXPECT_TRUE(gameManager->isGameOver());
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("Tie"));
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("ammunition"));
}

TEST_F(GameManagerTest, BothTanksDestroyed) {
    // Destroy both tanks
    tank1->destroy();
    tank2->destroy();
    
    // Process a step
    gameManager->processStep();
    
    // Game should be over with a tie
    EXPECT_TRUE(gameManager->isGameOver());
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("Tie"));
    EXPECT_THAT(gameManager->getGameResult(), ::testing::HasSubstr("Both tanks"));
} 