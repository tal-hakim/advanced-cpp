//
// Created by talta on 24/06/2025.
//

#ifndef SIMULATOR_COMPARATIVESIMULATOR_H
#define SIMULATOR_COMPARATIVESIMULATOR_H

#include <filesystem>
#include "Simulator.h"

class ComparativeSimulator : public Simulator{
private:
    const std::string& mapFile;
    const std::string& gameManagersFolder;
public:
    ComparativeSimulator(
            const std::string& mapFile,
            const std::string& gameManagersFolder,
            const std::string& alg1File,
            const std::string& alg2File,
            bool verbose
    ) : Simulator(verbose),
        gameManagersFolder(gameManagersFolder),
        mapFile(mapFile)
    {
        gameManagerSONames = getSoFilesInFolder(gameManagersFolder);
        mapsNames = {mapFile};
        if (haveSameFilename(alg1File, alg2File)){
            algorithmsSONames = {alg1File};
        }
        else{
            algorithmsSONames = {alg1File, alg2File};
        }
    }

    bool haveSameFilename(const std::string& path1, const std::string& path2) {
        std::filesystem::path p1(path1);
        std::filesystem::path p2(path2);
        return p1.filename() == p2.filename();
    }

    void buildGameContainers() override;

    void logResults() override;
};


#endif //SIMULATOR_COMPARATIVESIMULATOR_H
