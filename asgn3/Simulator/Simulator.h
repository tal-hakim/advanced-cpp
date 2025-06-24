//
// Created by talta on 23/06/2025.
//

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H
#include "../UserCommon/BoardSatelliteView.h"
#include <string>
#include <fstream>
#include <memory>
#include "../Common/AbstractGameManager.h"
#include "../Common/TankAlgorithm.h"
#include "../Common/Player.h"
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "GameContainer.h"
#include "BoardInitInfo.h"

using std::string;
class Simulator {

private:

    void setup(); // TODO: Goal: build the games that should run according to mode == gameContainers

    virtual void buildGameContainers() = 0;

    virtual void logResults() = 0;

    void runAllGames();

    void closeAllSharedObjects();

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
    Simulator(bool verbose) : verbose(verbose) {
    };
    void simulate();


};


#endif //SIMULATOR_SIMULATOR_H
