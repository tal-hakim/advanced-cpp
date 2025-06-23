//
// Created by talta on 23/06/2025.
//

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H
#include "../UserCommon/BoardSatelliteView.h"
#include <string>
#include <fstream>
#include <memory>
using namespace UserCommon_322213836_212054837;
using std::string;
class Simulator {
    struct BoardInitInfo {
        int width = 0;
        int height = 0;
        int maxSteps = 0;
        int numShells = 0;
        std::unique_ptr<BoardSatelliteView> satelliteView;
        string mapName;
    };
public:


private:
    std::vector<BoardInitInfo> boards;

    BoardInitInfo readMapFromFile(const string& inputFile);

    void readAllMapsInDirectory(const std::string &dirPath);


};


#endif //SIMULATOR_SIMULATOR_H
