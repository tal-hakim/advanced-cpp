//
// Created by talta on 03/08/2025.
//

#ifndef ASGN3_ERRORLOGGER_H
#define ASGN3_ERRORLOGGER_H


#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

class ErrorLogger {
private:
    std::unordered_map<std::string, std::vector<std::string>> errorsByMap;
    std::string currentMap;
    std::ofstream logFile;

public:
    ErrorLogger() = default;

    void logMapName(const std::string& mapName);
    void logMapToErrors();
    void logInputError(const std::string& message);

    void writeErrorsToFile(const std::string& filename = "input_errors.txt");
    void printAllErrors() const;
    bool hasErrors() const;

    ~ErrorLogger();
};

#endif //ASGN3_ERRORLOGGER_H
