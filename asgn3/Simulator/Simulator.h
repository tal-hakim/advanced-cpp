//
// Created by talta on 23/06/2025.
//

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H
#include "../UserCommon/BoardSatelliteView.h"
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <set>
#include <algorithm>
#include <filesystem>
#include <dlfcn.h>
#include <iostream>
#include "../common/AbstractGameManager.h"
#include "../common/TankAlgorithm.h"
#include "../common/Player.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "GameContainer.h"
#include "BoardInitInfo.h"
#include <optional>
#include <vector>
#include <memory>
#include "../common/GameResult.h"
#include "../UserCommon/utils/timeUtil.h"
#include <atomic>
#include <thread>

using namespace UserCommon_322213836_212054837;

using std::string;
class Simulator {

private:

    int numThreads;

    void setup();

    virtual void buildGameContainers() = 0;

    virtual void logResults() = 0;

    void runAllGames();

    void closeAllSharedObjects();

    void runGamesWorker(std::atomic<size_t>& nextIndex);


protected:
    std::vector<BoardInitInfo> boards;
    std::vector<void*> handles;
    std::vector<GameContainer> gameContainers;
    bool verbose;
    std::vector<string> algorithmsSONames;
    std::vector<string> gameManagerSONames;
    std::vector<string> mapsNames;
    BoardInitInfo readMapFromFile(const string& inputFile);

    void readAllMaps();

    std::vector<string> getFilenamesInFolder(const string &folderName);

    void loadSharedObject(const string &soName);

    void loadAlgorithmSharedObjectsFromFiles();
    void loadGameManagerSharedObjectsFromFiles();

public:

    Simulator(bool verbose, int numThreads) : verbose(verbose), numThreads(numThreads) {
    };
    void simulate();


    static string gameResultMessage(const GameResult &result);

    static std::string getReasonString(GameResult::Reason reason);

    static std::vector<string> getMapAsStrings(const SatelliteView &view, size_t width, size_t height);

    std::vector<string> getSoFilesInFolder(const string &folderName);
    struct MissingFilesException {
        std::string message;
    };
};


#endif //SIMULATOR_SIMULATOR_H
