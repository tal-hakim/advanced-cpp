#include "game/Logger.h"
#include <iostream>
#include <filesystem>

Logger::Logger(const std::string& inputFile)

{
    std::filesystem::path inputPath(inputFile);
    inputFilename = inputPath.filename().string();
    std::string outputFile = "output_" + inputFilename;
    outputLog.open(outputFile);
}

Logger::~Logger() {
    flushStepLog();
    if (outputLog.is_open()) outputLog.close();
    if (inputErrorLog.is_open()) inputErrorLog.close();
}

void Logger::logAction(int playerId, const std::string& action) {
    stepLogs.push_back("Player " + std::to_string(playerId) +
                       " - " + action);
}

void Logger::logStepNum(int step){
    stepLogs.push_back("\nGame Step #" + std::to_string(step + 1));
}

void Logger::logBadStep(int playerId, const std::string& reason) {
    stepLogs.push_back("Bad Step by Player " + std::to_string(playerId) + ": " + reason);
}

void Logger::logResult(const std::string& result)  {
    stepLogs.push_back("\n====== Game Over ====== \n" + result);
}

void Logger::logGameStart()  {
    stepLogs.push_back("====== Game Start ======");
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
