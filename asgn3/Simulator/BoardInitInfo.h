//
// Created by talta on 24/06/2025.
//

#ifndef SIMULATOR_BOARDINITINFO_H
#define SIMULATOR_BOARDINITINFO_H
#include "../UserCommon/BoardSatelliteView.h"
#include <memory>
#include <string>

using namespace UserCommon_322213836_212054837;

struct BoardInitInfo {
    int width = 0;
    int height = 0;
    int maxSteps = 0;
    int numShells = 0;
    std::unique_ptr<BoardSatelliteView> satelliteView;
    std::string mapName;
};
#endif //SIMULATOR_BOARDINITINFO_H
