#ifndef ASGN3_LOGGER_H
#define ASGN3_LOGGER_H

#include <string>
#include <fstream>
#include <vector>

namespace GameManager_322213836_212054837 {

class Logger {
private:
    std::ofstream outputLog;
    std::ofstream inputErrorLog;
    std::string inputFilename;
    mutable std::vector<std::string> stepLogs;
    std::vector<std::string> currentActions;  // Member variable to store actions
    bool hasInputErrors = false;

public:
    Logger(){};
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logResult(const std::string& result);
    void logInputError(const std::string& message);

    void flushStepLog();
    void finalize();

    void logStepNum(int step);
    void setLogFile(const std::string& inputFile);
    // Action logging methods
    void addAction(const std::string& action);  // Add a single action
    void clearActions();  // Clear the current actions
    void logActions();  // Log the current actions and clear them
    void appendToAction(size_t index, const std::string& str);  // Append to a specific action
};

} // namespace GameManager_322213836_212054837

#endif // ASGN3_LOGGER_H 