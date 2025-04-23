#include <gtest/gtest.h>
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Wall.h"
#include "../include/Mine.h"

class BoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a board before each test
        board = new Board(10, 8);
    }

    void TearDown() override {
        // Delete the board after each test
        delete board;
    }

    Board* board;
};

TEST_F(BoardTest, Dimensions) {
    EXPECT_EQ(board->getWidth(), 10);
    EXPECT_EQ(board->getHeight(), 8);
}

TEST_F(BoardTest, WrapPosition) {
    // Test wrapping in different directions
    Position pos1(12, 5);  // Right edge wrap
    Position wrapped1 = board->wrapPosition(pos1);
    EXPECT_EQ(wrapped1.x, 2);
    EXPECT_EQ(wrapped1.y, 5);
    
    Position pos2(-2, 3);  // Left edge wrap
    Position wrapped2 = board->wrapPosition(pos2);
    EXPECT_EQ(wrapped2.x, 8);
    EXPECT_EQ(wrapped2.y, 3);
    
    Position pos3(4, -1);  // Top edge wrap
    Position wrapped3 = board->wrapPosition(pos3);
    EXPECT_EQ(wrapped3.x, 4);
    EXPECT_EQ(wrapped3.y, 7);
    
    Position pos4(7, 10);  // Bottom edge wrap
    Position wrapped4 = board->wrapPosition(pos4);
    EXPECT_EQ(wrapped4.x, 7);
    EXPECT_EQ(wrapped4.y, 2);
    
    Position pos5(-3, -2);  // Multiple wraps
    Position wrapped5 = board->wrapPosition(pos5);
    EXPECT_EQ(wrapped5.x, 7);
    EXPECT_EQ(wrapped5.y, 6);
}

TEST_F(BoardTest, PlaceAndRetrieveObject) {
    // Place a tank on the board
    Tank* tank = new Tank(Position(3, 4), 1);
    board->placeObject(tank);
    
    // Retrieve the object from the board
    GameObject* retrieved = board->getObjectAt(Position(3, 4));
    EXPECT_EQ(retrieved, tank);
    
    // Check that the location is now occupied
    EXPECT_TRUE(board->isOccupied(Position(3, 4)));
    
    // Check that a different location is not occupied
    EXPECT_FALSE(board->isOccupied(Position(5, 5)));
    
    delete tank;  // Clean up
}

TEST_F(BoardTest, RemoveObject) {
    // Place a wall on the board
    Wall* wall = new Wall(Position(2, 2));
    board->placeObject(wall);
    
    // Verify it's there
    EXPECT_TRUE(board->isOccupied(Position(2, 2)));
    
    // Remove it
    board->removeObject(Position(2, 2));
    
    // Verify it's gone
    EXPECT_FALSE(board->isOccupied(Position(2, 2)));
    
    delete wall;  // Clean up
}

TEST_F(BoardTest, WrapObjectPosition) {
    // Place object outside board boundaries
    Mine* mine = new Mine(Position(15, 9));
    board->placeObject(mine);
    
    // The object should be wrapped to a valid position
    EXPECT_EQ(mine->getPosition().x, 5);
    EXPECT_EQ(mine->getPosition().y, 1);
    
    // Verify it's at the wrapped position
    EXPECT_TRUE(board->isOccupied(Position(5, 1)));
    
    delete mine;  // Clean up
} 