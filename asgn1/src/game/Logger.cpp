#include "game/Logger.h"
#include <iostream>
#include <filesystem>

Logger::Logger(const std::string& inputFile)

{
    std::filesystem::path inputPath(inputFile);
    inputFilename = inputPath.filename().string();
    std::string outputFile = "../logs/log_for_" + inputFilename;
    std::cout << std::filesystem::current_path() << std::endl;
    outputLog.open(outputFile);
}

Logger::~Logger() {
    flushStepLog();
    if (outputLog.is_open()) outputLog.close();
    if (inputErrorLog.is_open()) inputErrorLog.close();
}

void Logger::logAction(int playerId, const std::string& action) {
    std::cout << ": Player " << playerId
              << " - " << action << std::endl;
    stepLogs.push_back("Player " + std::to_string(playerId) +
                       " - " + action);
}

void Logger::logStepNum(int step){
    stepLogs.push_back("\nGame Step #" + std::to_string(step));
}

void Logger::logBadStep(int playerId, const std::string& reason) {
    std::string msg = "Bad Step by Player " + std::to_string(playerId) + ": " + reason;
    std::cout << msg << std::endl;
    stepLogs.push_back("Bad Step by Player " + std::to_string(playerId) + ": " + reason);
}

void Logger::logResult(const std::string& result)  {
    std::cout << "Game over" << result << std::endl;
    stepLogs.push_back("\n====== Game Over ====== \n" + result);
}

void Logger::logInputError(const std::string& message) {
    if (!inputErrorLog.is_open()) {
        inputErrorLog.open("input_errors.txt");
        hasInputErrors = true;
    }
    inputErrorLog << "Error: " << message << "\n";
}

void Logger::flushStepLog() {
    for (const std::string& log : stepLogs) {
        outputLog << log << "\n";
    }
    stepLogs.clear();
}

void Logger::finalize() {
    flushStepLog();
    if (hasInputErrors && inputErrorLog.is_open()) {
        inputErrorLog.close();
    }
}
