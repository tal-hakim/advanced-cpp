#include "game/Logger.h"
#include <iostream>
#include <filesystem>

Logger::Logger(const std::string& inputFile)
        : inputFilename(inputFile)
{
    std::string outputFile = "output_" + inputFile;
    outputLog.open(outputFile);
}

Logger::~Logger() {
    flushStepLog();
    if (outputLog.is_open()) outputLog.close();
    if (inputErrorLog.is_open()) inputErrorLog.close();
}

void Logger::logStep(int stepNumber, int playerId, const std::string& action) {
    stepLogs.push_back("Step " + std::to_string(stepNumber) +
                       ": Player " + std::to_string(playerId) +
                       " - " + action);
}


void Logger::logBadStep(int playerId, const std::string& reason) {
    stepLogs.push_back("Bad Step by Player " + std::to_string(playerId) + ": " + reason);
}

void Logger::logResult(const std::string& result) const {
    stepLogs.push_back("Game Over: " + result);
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
