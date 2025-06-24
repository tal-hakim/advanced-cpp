//
// Created by talta on 24/06/2025.
//

#ifndef SIMULATOR_COMPETITIVESIMULATOR_H
#define SIMULATOR_COMPETITIVESIMULATOR_H
#include "Simulator.h"


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
        algorithmsSONames = getFilenamesInFolder(algorithmsFolder);
        gameManagerSONames = {gameManagerFile};
        mapsNames = getFilenamesInFolder(mapsFolder);
    };

    void readAllMaps();

    void buildGameContainers() override;
};


#endif //SIMULATOR_COMPETITIVESIMULATOR_H
