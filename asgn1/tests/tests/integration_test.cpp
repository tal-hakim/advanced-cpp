#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>

class IntegrationTest : public ::testing::Test {
protected:
    // Helper function to execute a command and capture its output and exit code
    std::pair<std::string, int> executeCommand(const std::string& command) {
        std::array<char, 128> buffer;
        std::string result;
        
        #ifdef _WIN32
        // Windows implementation
        std::string cmd = command + " 2>&1";
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) {
            return {"Failed to execute command", -1};
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
        int exitCode = _pclose(pipe);
        #else
        // Unix implementation
        std::string cmd = command + " 2>&1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            return {"Failed to execute command", -1};
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
        int status = pclose(pipe);
        int exitCode = WEXITSTATUS(status);
        #endif
        
        return {result, exitCode};
    }
    
    // Create a simple valid board file for testing
    void createValidBoardFile() {
        std::ofstream file("valid_board.txt");
        file << "5 4\n"
             << "#####\n"
             << "#1 2#\n"
             << "# @ #\n"
             << "#####";
        file.close();
    }
    
    // Create a board file with errors for testing
    void createErrorBoardFile() {
        std::ofstream file("error_board.txt");
        file << "5 4\n"
             << "###\n"     // Too short
             << "#1X2#\n"    // Unknown char
             << "# @ #\n"
             << "#####";
        file.close();
    }
    
    void SetUp() override {
        // Create test board files
        createValidBoardFile();
        createErrorBoardFile();
        
        // Make sure we have the proper executable name based on platform
        #ifdef _WIN32
        executablePath = "tank_game.exe";
        #else
        executablePath = "./tank_game";
        #endif
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("valid_board.txt");
        std::remove("error_board.txt");
        std::remove("input_errors.txt");
    }
    
    std::string executablePath;
};

TEST_F(IntegrationTest, NoCommandLineArgs) {
    auto [output, exitCode] = executeCommand(executablePath);
    
    // Check exit code (should fail)
    EXPECT_NE(exitCode, 0);
    
    // Check output (should mention usage)
    EXPECT_THAT(output, ::testing::HasSubstr("Usage:"));
}

TEST_F(IntegrationTest, NonExistentFile) {
    auto [output, exitCode] = executeCommand(executablePath + " nonexistent_file.txt");
    
    // Check exit code (should fail)
    EXPECT_NE(exitCode, 0);
    
    // Check output (should mention error opening file)
    EXPECT_THAT(output, ::testing::HasSubstr("Could not open file"));
    EXPECT_THAT(output, ::testing::HasSubstr("Failed to create board"));
}

TEST_F(IntegrationTest, ValidFile) {
    auto [output, exitCode] = executeCommand(executablePath + " valid_board.txt");
    
    // Check exit code (should succeed)
    EXPECT_EQ(exitCode, 0);
    
    // Check output (should show successful parsing)
    EXPECT_THAT(output, ::testing::HasSubstr("Parsing completed successfully"));
    EXPECT_THAT(output, ::testing::HasSubstr("Tank 1 information"));
    EXPECT_THAT(output, ::testing::HasSubstr("Tank 2 information"));
}

TEST_F(IntegrationTest, FileWithErrors) {
    auto [output, exitCode] = executeCommand(executablePath + " error_board.txt");
    
    // Check exit code (should succeed with warnings)
    EXPECT_EQ(exitCode, 0);
    
    // Check output (should mention errors)
    EXPECT_THAT(output, ::testing::HasSubstr("Encountered some issues"));
    
    // Verify that input_errors.txt was created
    std::ifstream errorFile("input_errors.txt");
    EXPECT_TRUE(errorFile.good());
    
    // Read and check contents of input_errors.txt
    std::string errorContent((std::istreambuf_iterator<char>(errorFile)),
                             std::istreambuf_iterator<char>());
    
    EXPECT_THAT(errorContent, ::testing::HasSubstr("shorter"));
    EXPECT_THAT(errorContent, ::testing::HasSubstr("Unknown symbol"));
} 