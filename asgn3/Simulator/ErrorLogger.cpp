#include "ErrorLogger.h"

void ErrorLogger::logMapName(const std::string& mapName) {
    currentMap = mapName;
    if (errorsByMap.find(mapName) == errorsByMap.end()) {
        errorsByMap[mapName] = {};  // ensure vector exists
    }
}


void ErrorLogger::logInputError(const std::string& message) {
    if (!currentMap.empty()) {
        std::string formatted = "[Input Error] " + message;
        errorsByMap[currentMap].push_back(formatted);
    }
}

void ErrorLogger::writeErrorsToFile(const std::string& filename) {
    if (!hasErrors()) return;

    std::ofstream errFile(filename);  // overwrite
    if (!errFile.is_open()) return;

    for (const auto& [mapName, errors] : errorsByMap) {
        if (errors.empty()){
            continue;
        }
        errFile << "Map: " << mapName << "\n";
        errFile << "Recovered Errors:\n";
        for (const auto& err : errors) {
            errFile << "- " << err << "\n";
        }
        errFile << "\n";
    }

    errorsByMap.clear();
}

bool ErrorLogger::hasErrors() const {
    for (const auto& [_, errors] : errorsByMap) {
        if (!errors.empty()) return true;
    }
    return false;
}

void ErrorLogger::printAllErrors() const {
    for (const auto& [mapName, errors] : errorsByMap) {
        std::cout << "Map: " << mapName << "\n";
        for (const auto& err : errors) {
            std::cout << "- " << err << "\n";
        }
    }
}

ErrorLogger::~ErrorLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}