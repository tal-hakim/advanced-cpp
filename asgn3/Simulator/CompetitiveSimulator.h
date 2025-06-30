//
// Created by talta on 24/06/2025.
//

#ifndef SIMULATOR_COMPETITIVESIMULATOR_H
#define SIMULATOR_COMPETITIVESIMULATOR_H
#include "Simulator.h"
#include <vector>
#include <string>
#include <cassert>
#include "GameContainer.h"


// Competition between multiple ALGS. only one GameManager

class CompetitiveSimulator : public Simulator {
private:
    const std::string& mapsFolder;
    const std::string& gameManagerFile;
    const std::string& algorithmsFolder;

    void logResults() override;

public:
    CompetitiveSimulator(const std::string &mapsFolder, const std::string &gameManagerFile,
                         const std::string &algsFolder, bool verbose) : Simulator(verbose),
                         gameManagerFile(gameManagerFile), mapsFolder(mapsFolder), algorithmsFolder(algsFolder) {
        algorithmsSONames = getSoFilesInFolder(algorithmsFolder);
        if (algorithmsSONames.size() < 2)
        {
            throw MissingFilesException("Not enough algorithms in folder: " + algsFolder);
        }
        gameManagerSONames = {gameManagerFile};
        mapsNames = getFilenamesInFolder(mapsFolder);
        if (mapsNames.empty()) throw MissingFilesException("No maps in folder: " + mapsFolder);
    };

    void readAllMaps();

    void buildGameContainers() override;
};


#endif //SIMULATOR_COMPETITIVESIMULATOR_H
