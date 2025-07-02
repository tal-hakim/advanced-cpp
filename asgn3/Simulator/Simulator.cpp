//
// Created by talta on 23/06/2025.
//

#include "Simulator.h"



std::vector<std::string> Simulator::getFilenamesInFolder(const std::string& folderName) {
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(folderName)) {
        if (entry.is_regular_file())
            filenames.push_back(entry.path().string()); // FULL PATH
    }
    return filenames;
}

std::vector<std::string> Simulator::getSoFilesInFolder(const std::string& folderName) {
    std::vector<std::string> soFiles;
    for (const auto& entry : std::filesystem::directory_iterator(folderName)) {
        if (entry.is_regular_file()) {
            const auto& path = entry.path();
            if (path.extension() == ".so") {
                soFiles.push_back(path.string()); // Full path
            }
        }
    }
    return soFiles;
}


BoardInitInfo Simulator::readMapFromFile(const string& inputFile) {
    // TODO: handle input errors
    BoardInitInfo info;

    std::ifstream file(inputFile);
    if (!file.is_open()) {
        // handle error, log, etc.
        return info;
    }
    std::filesystem::path p(inputFile);
    info.mapFilename = p.stem().string();

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
    if (sscanf(line.c_str(), "MaxSteps=%zu", &info.maxSteps) != 1) return info;

    // Read NumShells
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%zu", &info.numShells) != 1) return info;

    // Read Rows
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%zu", &info.height) != 1) return info;

    // Read Cols
    if (!std::getline(file, line)) return info;
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%zu", &info.width) != 1) return info;

    if (info.height <= 0 || info.width <= 0) return info;

    // Read map lines
    std::vector<std::string> mapLines;
    while (std::getline(file, line)) {
        mapLines.push_back(line);
    }

    std::vector<std::vector<char>> boardView(info.width, std::vector<char>(info.height, ' '));
    for (size_t y = 0; y < info.height; ++y) {
        std::string currentLine = (y < mapLines.size()) ? mapLines[y] : "";
        for (size_t x = 0; x < info.width; ++x) {
            char cell = (x < currentLine.size()) ? currentLine[x] : ' ';
            boardView[x][y] = cell;
        }
    }

    info.satelliteView = std::make_unique<UserCommon_322213836_212054837::BoardSatelliteView>(std::move(boardView));
    return info;
}





void Simulator::loadSharedObject(const std::string& soName) {
    void* handle = dlopen(soName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        throw std::runtime_error(dlerror()); // You can handle/log error differently if you prefer
    }
    handles.add(handle);
}

void Simulator::loadGameManagerSharedObjectsFromFiles() {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();

    for (const auto& filePath : gameManagerSONames) {
        if (std::filesystem::path(filePath).extension() == ".so") {
            std::string filename = std::filesystem::path(filePath).filename().string();
            std::string managerName = filename.substr(0, filename.find_last_of('.')); // remove .so

            registrar.createAlgorithmFactoryEntry(managerName);

            try {
                loadSharedObject(filePath);
                std::cout << "Loaded game manager: " << filePath << std::endl;
            } catch (const std::exception& ex) {
                std::cerr << "Failed to load " << filePath << ": " << ex.what() << std::endl;
                registrar.removeLast();
                continue;
            }

            try {
                registrar.validateLastRegistration();
            } catch (GameManagerRegistrar::BadRegistrationException& e) {
                std::cout << "---------------------------------" << std::endl;
                std::cout << "BadRegistrationException for: " << managerName << std::endl;
                std::cout << "Name as registered: " << e.name << std::endl;
                std::cout << "Has GameManager factory? " << std::boolalpha << e.hasGameManagerFactory << std::endl;
                std::cout << "---------------------------------" << std::endl;
                registrar.removeLast();
            }
        }
    }
}


// Function to load all .so files in a folder
void Simulator::loadAlgorithmSharedObjectsFromFiles() {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    for (const auto& filePath : algorithmsSONames) {
        if (std::filesystem::path(filePath).extension() == ".so") {
            std::string filename = std::filesystem::path(filePath).filename().string();
            std::string algName = filename.substr(0, filename.find_last_of('.')); // remove .so

            registrar.createAlgorithmFactoryEntry(algName);

            try {
                loadSharedObject(filePath);
                std::cout << "Loaded algorithm: " << filePath << std::endl;
            } catch (const std::exception& ex) {
                std::cerr << "Failed to load " << filePath << ": " << ex.what() << std::endl;
                registrar.removeLast();
                continue;
            }

            try {
                registrar.validateLastRegistration();
            } catch (AlgorithmRegistrar::BadRegistrationException& e) {
                std::cout << "---------------------------------" << std::endl;
                std::cout << "BadRegistrationException for: " << algName << std::endl;
                std::cout << "Name as registered: " << e.name << std::endl;
                std::cout << "Has tank algorithm factory? " << std::boolalpha << e.hasTankAlgorithmFactory << std::endl;
                std::cout << "Has Player factory? " << std::boolalpha << e.hasPlayerFactory << std::endl;
                std::cout << "---------------------------------" << std::endl;
                registrar.removeLast();
            }
        }
    }
}


void Simulator::runGamesWorker(std::atomic<size_t>& nextIndex) {
    size_t nGames = gameContainers.size(); // Just get it here!
    while (true) {
        size_t i = nextIndex.fetch_add(1, std::memory_order_relaxed);
        if (i >= nGames) break;
        gameContainers[i].startGame();
    }
}

void Simulator::runAllGames() {
    size_t nGames = gameContainers.size();
    int threadsToUse = numThreads;

    if (threadsToUse == 1) {
        // Single-threaded execution
        for (auto& game : gameContainers)
            game.startGame();
        return;
    }

    // Make sure not to spawn more threads than games
    threadsToUse = std::min<int>(threadsToUse, nGames);

    // Shared atomic counter for all threads
    std::atomic<size_t> nextIndex{0};
    std::vector<std::jthread> workers;

    // Start the threads
    for (int t = 0; t < threadsToUse; ++t)
        workers.emplace_back([this, &nextIndex]() {
            runGamesWorker(nextIndex);
        });

    // All threads automatically join at the end of scope (std::jthread feature)
    for (auto& t : workers)
        t.join();
}

void Simulator::simulate() {
    setup();
    runAllGames();
    logResults();
//    closeAllSharedObjects();
}

//void Simulator::closeAllSharedObjects() {
//    for (void* handle : handles) {
//        if (handle) {
//            dlclose(handle);
//        }
//    }
//    handles.clear();
//}

void Simulator::setup() {
    readAllMaps();
    loadAlgorithmSharedObjectsFromFiles();
    loadGameManagerSharedObjectsFromFiles();
    buildGameContainers();
}

void Simulator::readAllMaps() {
    for (const auto& filePath : mapsNames) {
        BoardInitInfo info = readMapFromFile(filePath);
        if (info.satelliteView) {
            boards.push_back(std::move(info));
        } else {
            // std::cerr << "Failed to read board: " << filePath << std::endl;
        }
    }
}


std::string Simulator::gameResultMessage(const GameResult& result) {
    std::ostringstream oss;
    if (result.winner == 0)
        oss << "Tie: " << getReasonString(result.reason);
    else
        oss << "Player " << result.winner << " wins: " << getReasonString(result.reason);
    return oss.str();
}

std::string Simulator::getReasonString(GameResult::Reason reason) {
    switch (reason) {
        case GameResult::ALL_TANKS_DEAD: return "All tanks dead";
        case GameResult::MAX_STEPS:      return "Max steps";
        case GameResult::ZERO_SHELLS:    return "Zero shells";
        default:                         return "Unknown";
    }
}

// Helper to get the final map as vector of strings
std::vector<std::string> Simulator::getMapAsStrings(const SatelliteView& view, size_t width, size_t height) {
    std::vector<std::string> mapLines;
    for (size_t y = 0; y < height; ++y) {
        std::string row;
        for (size_t x = 0; x < width; ++x) {
            row += view.getObjectAt(x, y);
        }
        mapLines.push_back(row);
    }
    return mapLines;
}

