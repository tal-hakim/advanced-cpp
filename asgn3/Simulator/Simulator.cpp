//
// Created by talta on 23/06/2025.
//


#include <algorithm>
#include <filesystem>
#include "Simulator.h"

Simulator::BoardInitInfo Simulator::readMapFromFile(const string& inputFile) {
    // TODO: handle input errors
    BoardInitInfo info;

    std::ifstream file(inputFile);
    if (!file.is_open()) {
        // handle error, log, etc.
        return info;
    }

    std::string line;

    // Read map name/description (Line 1)
    if (!std::getline(file, info.mapName))
        return info;

    // Helper lambda to remove spaces
    auto removeSpaces = [](std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    };

    // Read MaxSteps
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "MaxSteps=%d", &info.maxSteps) != 1) return info;

    // Read NumShells
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%d", &info.numShells) != 1) return info;

    // Read Rows
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%d", &info.height) != 1) return info;

    // Read Cols
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%d", &info.width) != 1) return info;

    if (info.height <= 0 || info.width <= 0) return info;

    // Read map lines
    std::vector<std::string> mapLines;
    while (std::getline(file, line)) {
        mapLines.push_back(line);
    }

    std::vector<std::vector<char>> boardView(info.height, std::vector<char>(info.width, ' '));
    for (int y = 0; y < info.height; ++y) {
        std::string currentLine = (y < (int)mapLines.size()) ? mapLines[y] : "";
        for (int x = 0; x < info.width; ++x) {
            char cell = (x < (int)currentLine.size()) ? currentLine[x] : ' ';
            boardView[y][x] = cell;
        }
    }

    info.satelliteView = std::make_unique<UserCommon_322213836_212054837::BoardSatelliteView>(std::move(boardView));
    return info;
}


/* TODO: func that returns all filenames inside folder
 * TODO: func that loads a .so
 * TODO: func that goes over vec of files and loads .so
 * TODO: CompareSim and CompetitiveSim derived classes
 * TODO: log results
 * TODO: log errors
 * TODO: main
 * TODO: threads :(*/



void Simulator::readAllMapsInDirectory(const std::string &dirPath) {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();

            BoardInitInfo info = readMapFromFile(filePath);
            if (info.satelliteView) { // Only add if loaded successfully
                boards.push_back(std::move(info));
            } else {
                // Optionally log/handle invalid file
                // std::cerr << "Failed to read board: " << filePath << std::endl;
            }
        }
    }
}
