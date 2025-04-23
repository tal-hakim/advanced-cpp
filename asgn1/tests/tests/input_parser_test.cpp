#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <sstream>
#include <string>
#include "../include/InputParser.h"

class InputParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = new InputParser();
    }

    void TearDown() override {
        delete parser;
    }

    // Create a temporary board file with given content
    std::string createTempBoardFile(const std::string& content) {
        std::string filename = "temp_board_test.txt";
        std::ofstream file(filename);
        file << content;
        file.close();
        return filename;
    }

    void removeTempFile(const std::string& filename) {
        std::remove(filename.c_str());
    }

    InputParser* parser;
};

TEST_F(InputParserTest, ParseValidBoard) {
    // Create a simple valid board
    std::string boardContent = 
        "5 4\n"
        "#####\n"
        "#1 2#\n"
        "# @ #\n"
        "#####";
    
    std::string filename = createTempBoardFile(boardContent);
    
    // Parse the board
    Board* board = parser->parseInputFile(filename);
    
    // Check that parsing succeeded
    ASSERT_NE(board, nullptr);
    
    // Check board dimensions
    EXPECT_EQ(board->getWidth(), 5);
    EXPECT_EQ(board->getHeight(), 4);
    
    // Check that objects were placed correctly
    EXPECT_TRUE(board->isOccupied(Position(0, 0)));  // Wall at top-left
    EXPECT_TRUE(board->isOccupied(Position(1, 1)));  // Tank 1 position
    EXPECT_TRUE(board->isOccupied(Position(3, 1)));  // Tank 2 position
    EXPECT_TRUE(board->isOccupied(Position(2, 2)));  // Mine position
    
    // Check tank pointers are valid
    EXPECT_NE(parser->getTank1(), nullptr);
    EXPECT_NE(parser->getTank2(), nullptr);
    
    // Verify tank properties
    EXPECT_EQ(parser->getTank1()->getPosition().x, 1);
    EXPECT_EQ(parser->getTank1()->getPosition().y, 1);
    EXPECT_EQ(parser->getTank1()->getDirection(), Direction::LEFT);
    
    EXPECT_EQ(parser->getTank2()->getPosition().x, 3);
    EXPECT_EQ(parser->getTank2()->getPosition().y, 1);
    EXPECT_EQ(parser->getTank2()->getDirection(), Direction::RIGHT);
    
    // Clean up
    delete board;
    removeTempFile(filename);
}

TEST_F(InputParserTest, HandleMultipleTanks) {
    // Create a board with multiple tanks for player 1
    std::string boardContent = 
        "5 4\n"
        "#####\n"
        "#1 2#\n"
        "#1@ #\n"
        "#####";
    
    std::string filename = createTempBoardFile(boardContent);
    
    // Parse the board
    Board* board = parser->parseInputFile(filename);
    
    // Check that parsing succeeded with errors
    ASSERT_NE(board, nullptr);
    EXPECT_TRUE(parser->hasErrors());
    
    // Check that the error message mentions multiple tanks
    auto messages = parser->getErrorMessages();
    EXPECT_THAT(messages, ::testing::Contains(::testing::HasSubstr("Multiple Tank 1")));
    
    // Verify only the first tank was used
    EXPECT_EQ(parser->getTank1()->getPosition().x, 1);
    EXPECT_EQ(parser->getTank1()->getPosition().y, 1);
    
    // Clean up
    delete board;
    removeTempFile(filename);
}

TEST_F(InputParserTest, HandleMalformedDimensions) {
    // Create a board with invalid dimensions
    std::string boardContent = "invalid\n#####\n#1 2#\n";
    
    std::string filename = createTempBoardFile(boardContent);
    
    // Parse the board
    Board* board = parser->parseInputFile(filename);
    
    // Check that parsing failed
    EXPECT_EQ(board, nullptr);
    EXPECT_TRUE(parser->hasErrors());
    
    // Check that the error message mentions dimensions
    auto messages = parser->getErrorMessages();
    EXPECT_THAT(messages, ::testing::Contains(::testing::HasSubstr("Failed to extract board dimensions")));
    
    // Clean up
    removeTempFile(filename);
}

TEST_F(InputParserTest, HandleMismatchedDimensions) {
    // Create a board with lines shorter than specified width
    std::string boardContent = 
        "5 4\n"
        "###\n"    // Too short
        "#1 2#\n"  // Correct
        "##\n"     // Too short
        "######";  // Too long
    
    std::string filename = createTempBoardFile(boardContent);
    
    // Parse the board
    Board* board = parser->parseInputFile(filename);
    
    // Check that parsing succeeded with errors
    ASSERT_NE(board, nullptr);
    EXPECT_TRUE(parser->hasErrors());
    
    // Check that the error messages mention line lengths
    auto messages = parser->getErrorMessages();
    EXPECT_THAT(messages, ::testing::Contains(::testing::HasSubstr("shorter")));
    EXPECT_THAT(messages, ::testing::Contains(::testing::HasSubstr("longer")));
    
    // Clean up
    delete board;
    removeTempFile(filename);
}

TEST_F(InputParserTest, HandleUnknownCharacters) {
    // Create a board with unknown characters
    std::string boardContent = 
        "5 4\n"
        "#####\n"
        "#1X2#\n"  // X is unknown
        "# @ #\n"
        "#####";
    
    std::string filename = createTempBoardFile(boardContent);
    
    // Parse the board
    Board* board = parser->parseInputFile(filename);
    
    // Check that parsing succeeded with errors
    ASSERT_NE(board, nullptr);
    EXPECT_TRUE(parser->hasErrors());
    
    // Check that the error message mentions the unknown character
    auto messages = parser->getErrorMessages();
    EXPECT_THAT(messages, ::testing::Contains(::testing::HasSubstr("Unknown symbol 'X'")));
    
    // Clean up
    delete board;
    removeTempFile(filename);
} 