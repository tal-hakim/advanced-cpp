//
// Created by talta on 23/06/2025.
//


#include <algorithm>
#include <filesystem>
#include <dlfcn.h>
#include <iostream>
#include "Simulator.h"



std::vector<std::string> Simulator::getFilenamesInFolder(const std::string& folderName) {
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(folderName)) {
        if (entry.is_regular_file())
            filenames.push_back(entry.path().string()); // FULL PATH
    }
    return filenames;
}


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


/* v func that returns all filenames inside folder
 * v func that loads a .so
 * v func that goes over vec of files and loads .so
 * TODO: CompareSim and CompetitiveSim derived classes
 * TODO: log results
 * TODO: log errors
 * TODO: main
 * TODO: threads :(
 * TODO: func that builds the list to run
 * TODO: make class "gameContainer" that holds: relevant gameManager, map ..., GameResult. has func run */



void Simulator::readAllMapsInDirectory(const std::string &dirPath) {
    auto files = getFilenamesInFolder(dirPath);
    for (const auto& filePath : files) {
        BoardInitInfo info = readMapFromFile(filePath);
        if (info.satelliteView) {
            boards.push_back(std::move(info));
        } else {
            // std::cerr << "Failed to read board: " << filePath << std::endl;
        }
    }
}

void Simulator::loadSharedObject(const std::string& soName) {
    void* handle = dlopen(soName.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error(dlerror()); // You can handle/log error differently if you prefer
    }
    handles.push_back(handle);
}


// Function to load all .so files in a folder
void Simulator::loadAllSharedObjectsInFolder(const std::string& folderName) {
    auto allFiles = getFilenamesInFolder(folderName);
    for (const auto& filePath : allFiles) {
        if (std::filesystem::path(filePath).extension() == ".so") {
            try {
                loadSharedObject(filePath);
                std::cout << "Loaded: " << filePath << std::endl;
            } catch (const std::exception& ex) {
                std::cerr << "Failed to load " << filePath << ": " << ex.what() << std::endl;
            }
        }
    }
}