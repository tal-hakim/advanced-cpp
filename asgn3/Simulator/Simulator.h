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
    std::vector<BoardInitInfo> boards;
    std::vector<void*> handles;
    std::vector<GameContainer> gameContainers;

    BoardInitInfo readMapFromFile(const string& inputFile);

    void readAllMapsInDirectory(const std::string &dirPath);

    virtual void buildSetup() = 0; // Goal: build the games that should run according to mode == gameContainers

    std::vector<string> getFilenamesInFolder(const string &folderName);

    void loadSharedObject(const string &soName);

    void loadAllSharedObjectsInFolder(const string &folderName);

public:
    void simulate(){};
};


#endif //SIMULATOR_SIMULATOR_H
