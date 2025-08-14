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

BoardInitInfo Simulator::readMapFromFile(const std::string& inputFile) {
    BoardInitInfo info;

    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Unrecoverable Error: Cannot open input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    std::filesystem::path p(inputFile);
    std::string mapName = p.stem().string();
    info.mapFilename = mapName;

    logger.logMapName(mapName);  // This writes "Map: <name>" into the log file

    std::string line;

    if (!std::getline(file, info.mapName)) {
        std::cerr << "Unrecoverable Error: Empty file or missing map name line. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    auto removeSpaces = [](std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    };

    if (!std::getline(file, line)) {
        std::cerr << "Unrecoverable Error: Missing MaxSteps line. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "MaxSteps=%zu", &info.maxSteps) != 1) {
        std::cerr << "Unrecoverable Error: Invalid MaxSteps format. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    if (!std::getline(file, line)) {
        std::cerr << "Unrecoverable Error: Missing NumShells line. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "NumShells=%zu", &info.numShells) != 1) {
        std::cerr << "Unrecoverable Error: Invalid NumShells format. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    if (!std::getline(file, line)) {
        std::cerr << "Unrecoverable Error: Missing Rows line. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Rows=%zu", &info.height) != 1) {
        std::cerr << "Unrecoverable Error: Invalid Rows format. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    if (!std::getline(file, line)) {
        std::cerr << "Unrecoverable Error: Missing Cols line. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }
    removeSpaces(line);
    if (sscanf(line.c_str(), "Cols=%zu", &info.width) != 1) {
        std::cerr << "Unrecoverable Error: Invalid Cols format. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    if (info.height == 0 || info.width == 0) {
        std::cerr << "Unrecoverable Error: Invalid map dimensions. In input file: " << inputFile << std::endl;
        info.valid = false;
        return info;
    }

    std::vector<std::string> mapLines;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        mapLines.push_back(line);
    }

    if (mapLines.size() > info.height) {
        logger.logInputError("Input file has more lines than declared height. Ignoring extra lines.");
    }

    std::vector<std::vector<char>> boardView(info.width, std::vector<char>(info.height, ' '));

    for (size_t y = 0; y < info.height; ++y) {
        std::string currentLine = (y < mapLines.size()) ? mapLines[y] : "";
        if (currentLine.empty() && y < mapLines.size()) {
            logger.logInputError("Empty line at y=" + std::to_string(y) + ". Filling with spaces.");
        }

        else if (currentLine.length() < info.width) {
            logger.logInputError("Line too short at y=" + std::to_string(y) + ". Padding with spaces.");
            currentLine += std::string(info.width - currentLine.length(), ' ');
        }
        else if (currentLine.length() > info.width) {
            logger.logInputError("Line too long at y=" + std::to_string(y) + ". Truncating.");
            currentLine = currentLine.substr(0, info.width);
        }

        for (size_t x = 0; x < info.width; ++x) {
            boardView[x][y] = currentLine[x];
        }
    }

    info.satelliteView = std::make_unique<UserCommon_322213836_212054837::BoardSatelliteView>(std::move(boardView));

    return info;
}


//BoardInitInfo Simulator::readMapFromFile(const string& inputFile) {
//    // TODO: handle input errors
//    BoardInitInfo info;
//
//    std::ifstream file(inputFile);
//    if (!file.is_open()) {
//        // handle error, log, etc.
//        return info;
//    }
//    std::filesystem::path p(inputFile);
//    info.mapFilename = p.stem().string();
//
//    std::string line;
//
//    // Read map name/description (Line 1)
//    if (!std::getline(file, info.mapName))
//        return info;
//
//    // Helper lambda to remove spaces
//    auto removeSpaces = [](std::string& str) {
//        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
//    };
//
//    // Read MaxSteps
//    if (!std::getline(file, line)) return info;
//    removeSpaces(line);
//    if (sscanf(line.c_str(), "MaxSteps=%zu", &info.maxSteps) != 1) return info;
//
//    // Read NumShells
//    if (!std::getline(file, line)) return info;
//    removeSpaces(line);
//    if (sscanf(line.c_str(), "NumShells=%zu", &info.numShells) != 1) return info;
//
//    // Read Rows
//    if (!std::getline(file, line)) return info;
//    removeSpaces(line);
//    if (sscanf(line.c_str(), "Rows=%zu", &info.height) != 1) return info;
//
//    // Read Cols
//    if (!std::getline(file, line)) return info;
//    removeSpaces(line);
//    if (sscanf(line.c_str(), "Cols=%zu", &info.width) != 1) return info;
//
//    if (info.height <= 0 || info.width <= 0) return info;
//
//    // Read map lines
//    std::vector<std::string> mapLines;
//    while (std::getline(file, line)) {
//        mapLines.push_back(line);
//    }
//
//    std::vector<std::vector<char>> boardView(info.width, std::vector<char>(info.height, ' '));
//    for (size_t y = 0; y < info.height; ++y) {
//        std::string currentLine = (y < mapLines.size()) ? mapLines[y] : "";
//        for (size_t x = 0; x < info.width; ++x) {
//            char cell = (x < currentLine.size()) ? currentLine[x] : ' ';
//            boardView[x][y] = cell;
//        }
//    }
//
//    info.satelliteView = std::make_unique<UserCommon_322213836_212054837::BoardSatelliteView>(std::move(boardView));
//    return info;
//}


void Simulator::loadGameManagerSharedObjectsFromFiles() {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    std::vector<std::string> loadedManagers;
    for (const auto& filePath : gameManagerSONames) {
        if (std::filesystem::path(filePath).extension() == ".so") {
            std::string filename = std::filesystem::path(filePath).filename().string();
            std::string managerName = filename.substr(0, filename.find_last_of('.')); // remove .so
            registrar.createGameManagerFactoryEntry(managerName);
            try {
                registrar.loadGameManagerSO(filePath);
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
                continue;
            }
            loadedManagers.push_back(filePath);
        }
    }
    gameManagerSONames = std::move(loadedManagers);
}


// Function to load all .so files in a folder
void Simulator::loadAlgorithmSharedObjectsFromFiles() {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    std::vector<std::string> loadedAlgorithms;

    for (const auto& filePath : algorithmsSONames) {
        if (std::filesystem::path(filePath).extension() == ".so") {
            std::string filename = std::filesystem::path(filePath).filename().string();
            std::string algName = filename.substr(0, filename.find_last_of('.')); // remove .so
            registrar.createAlgorithmFactoryEntry(algName);

            try {
                registrar.loadAlgorithmSO(filePath);
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
                continue;
            }
            loadedAlgorithms.push_back(filePath);
        }
    }
    algorithmsSONames = std::move(loadedAlgorithms); // now contains only the valid, loaded ones
}



void Simulator::runGamesWorker(std::atomic<size_t>& nextIndex) {
    // TEST THREADS
    std::lock_guard<std::mutex> lock(observed_mutex);
    observed_worker_threads.insert(std::this_thread::get_id());
    // END TEST THREADS
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
    // Make sure not to spawn more threads than games
    threadsToUse = std::min<int>(threadsToUse, nGames);

    if (threadsToUse == 1) {
        // Single-threaded execution
        for (auto& game : gameContainers)
            game.startGame();
        return;
    }

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
//    std::cout << "Observed " << observed_worker_threads.size() << " unique worker threads." << std::endl;
    assert(static_cast<int>(observed_worker_threads.size()) == threadsToUse); // num_threads is your configured worker count

}

void Simulator::simulate() {
    setup();
    runAllGames();
    logResults();
}


void Simulator::setup() {
    readAllMaps();
    if(boards.empty()){
        return;
    }
    loadAlgorithmSharedObjectsFromFiles();
    loadGameManagerSharedObjectsFromFiles();
    buildGameContainers();
}

void Simulator::readAllMaps() {
    for (const auto& filePath : mapsNames) {
        BoardInitInfo info = readMapFromFile(filePath);
        if (info.satelliteView && info.valid) {
            boards.push_back(std::move(info));
        } else {
            // std::cerr << "Failed to read board: " << filePath << std::endl;
        }
    }
    logger.writeErrorsToFile();
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

