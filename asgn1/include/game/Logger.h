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
    std::vector<std::string> stepLogs;
    bool hasInputErrors = false;

public:
    Logger(const std::string& inputFile);
    ~Logger();

    void logBadStep(int playerId, const std::string& reason);
    void logResult(const std::string& result) const;
    void logInputError(const std::string& message);

    void flushStepLog();
    void finalize();

    void logStep(int stepNumber, int playerId, const std::string &action);
};

#endif // LOGGER_H
