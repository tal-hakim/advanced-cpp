#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <vector>

class Logger {
private:
    std::ofstream outputLog;
    std::ofstream inputErrorLog;
    std::string inputFilename;
    mutable std::vector<std::string> stepLogs;
    std::vector<std::string> currentActions;  // Member variable to store actions
    bool hasInputErrors = false;

public:
    Logger(const std::string& inputFile);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void logBadStep(int playerId, const std::string& reason);
    void logResult(const std::string& result);
    void logInputError(const std::string& message);

    void flushStepLog();
    void finalize();

    void logAction(int playerId, const std::string &action);

    void logStepNum(int step);

    void logGameStart();

    // Action logging methods
    void addAction(const std::string& action);  // Add a single action
    void clearActions();  // Clear the current actions
    void logActions();  // Log the current actions and clear them
    void appendToAction(size_t index, const std::string& str);  // Append to a specific action
};

#endif // LOGGER_H
