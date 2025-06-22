#include "Logger.h"
#include <iostream>
#include <filesystem>

namespace GameManager_322213836_212054837 {

void Logger::setLogFile(const std::string &inputFile) {
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

void Logger::logStepNum(int step){
    stepLogs.push_back("\nGame Step #" + std::to_string(step + 1));
}

void Logger::logResult(const std::string& result) {
    stepLogs.push_back(result);
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

void Logger::addAction(const std::string& action) {
    currentActions.push_back(action);
}

void Logger::clearActions() {
    currentActions.clear();
}

void Logger::logActions() {
    if (currentActions.empty()) {
        return;
    }
    
    std::string combined;
    for (size_t i = 0; i < currentActions.size(); ++i) {
        combined += currentActions[i];
        if (i < currentActions.size() - 1) {
            combined += ", ";
        }
    }
    stepLogs.push_back(combined);
    clearActions();  // Clear after logging
}

void Logger::appendToAction(size_t index, const std::string& str) {
    if (index >= currentActions.size()) {
        return;  // Index out of bounds, do nothing
    }
    currentActions[index] += str;
}

} // namespace GameManager_322213836_212054837 