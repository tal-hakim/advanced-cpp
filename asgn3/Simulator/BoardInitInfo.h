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
    size_t width = 0;
    size_t height = 0;
    size_t maxSteps = 0;
    size_t numShells = 0;
    std::unique_ptr<BoardSatelliteView> satelliteView;
    std::string mapFilename;
    std::string mapName;
    bool valid = true;
};
#endif //SIMULATOR_BOARDINITINFO_H
