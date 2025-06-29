//
// Created by talta on 22/06/2025.
//
#include "ComparativeSimulator.h"
#include "CompetitiveSimulator.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Example usage:
        // ./simulator -comparative game_map=... game_managers_folder=... algorithms=... alg1=... alg2=... [-verbose]
        // ./simulator -competition game_maps_folder=... game_manager=... algorithms_folder=... [-verbose]

        bool verbose = false;
        std::string mode;
        std::string gameMap, gameManagersFolder, algorithmsFolder, gameMapsFolder, alg1, alg2, gameManagerFile;

        // Simple argument parsing (adapt as needed for your assignment's spec!)
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-comparative") mode = "comparative";
            else if (arg == "-competition") mode = "competition";
            else if (arg.find("game_map=") == 0) gameMap = arg.substr(9);
            else if (arg.find("game_managers_folder=") == 0) gameManagersFolder = arg.substr(21);
            else if (arg.find("algorithms_folder=") == 0) algorithmsFolder = arg.substr(19);
            else if (arg.find("game_maps_folder=") == 0) gameMapsFolder = arg.substr(17);
            else if (arg.find("algorithm1=") == 0) alg1 = arg.substr(11);
            else if (arg.find("algorithm2=") == 0) alg2 = arg.substr(11);
            else if (arg.find("game_manager=") == 0) gameManagerFile = arg.substr(13);
            else if (arg == "-verbose") verbose = true;
        }

        if (mode == "comparative") {
            if (gameMap.empty() || gameManagersFolder.empty() || alg1.empty() || alg2.empty()) {
                std::cerr << "Missing required arguments for comparative mode." << std::endl;
                return 1;
            }
            ComparativeSimulator sim(gameMap, gameManagersFolder, alg1, alg2, verbose);
            sim.simulate();
        }
        else if (mode == "competition") {
            if (gameMapsFolder.empty() || algorithmsFolder.empty() || gameManagerFile.empty()) {
                std::cerr << "Missing required arguments for competition mode." << std::endl;
                return 1;
            }
            CompetitiveSimulator sim(gameMapsFolder, gameManagerFile, algorithmsFolder, verbose);
            sim.simulate();
        }
        else {
            std::cerr << "Please specify mode: -comparative or -competition" << std::endl;
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 3;
    }
    return 0;
}
