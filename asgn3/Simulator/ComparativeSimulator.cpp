//
// Created by talta on 24/06/2025.
//

#include <iostream>
#include "ComparativeSimulator.h"

void ComparativeSimulator::buildGameContainers() {
    auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    BoardInitInfo& board = boards[0];

    // Assume algorithmsSONames[0] and algorithmsSONames[1] are your two algorithms

    auto& alg1Entry = *(algoRegistrar.begin() + 0);
    auto& alg2Entry = (algorithmsSONames.size() < 2) ? alg1Entry : *(algoRegistrar.begin() + 1);
    const std::string& alg1Name = alg1Entry.name();
    const std::string& alg2Name = alg2Entry.name();

    for (size_t gmIdx = 0; gmIdx < gameManagerSONames.size(); ++gmIdx) {
        auto& gmEntry = *(gmRegistrar.begin() + gmIdx);

        auto player1 = alg1Entry.createPlayer(PLAYER_1_ID, board.width, board.height, board.maxSteps, board.numShells);
        auto player2 = alg2Entry.createPlayer(PLAYER_2_ID, board.width, board.height, board.maxSteps, board.numShells);
        auto gameManager = gmEntry.createGameManager(verbose);

        gameContainers.emplace_back(
                board,
                std::move(gameManager),
                std::move(player1),
                std::move(player2),
                alg1Entry.getTankAlgorithmFactory(),
                alg2Entry.getTankAlgorithmFactory(),
                alg1Name,
                alg2Name,
                gmEntry.name()
        );
    }
}

void ComparativeSimulator::logResults(){
    if (gameContainers.empty()) return;

    // 1. Output file name
//    std::string outFile = gameManagersFolder + "/comparative_results_" + getCurrentTimeString() + ".txt";
    std::string outFile = gameManagersFolder + "/comparative_results.txt";
    std::ofstream out(outFile);
    bool toScreen = false;
    if (!out) {
        std::cerr << "Failed to open output file '" << outFile << "'. Printing to screen instead.\n";
        toScreen = true;
    }
    std::ostream& outStream = toScreen ? std::cout : out;

    // 2. Write first lines
    outStream << "game_map=" << mapFile << "\n";
    outStream << "algorithm1=" << gameContainers[0].getAlg1Name() << "\n";
    outStream << "algorithm2=" << gameContainers[0].getAlg2Name() << "\n\n";

    // 3. Prepare for grouping
    std::vector<bool> handled(gameContainers.size(), false);
    size_t left = gameContainers.size();
    // 4. Group by identical results using isSameResult
    while (left > 0) {
        std::vector<size_t> thisGroup;

        // Find the first unhandled result as a group leader
        size_t leaderIdx = 0;
        while (leaderIdx < gameContainers.size() && handled[leaderIdx]) ++leaderIdx;
        if (leaderIdx == gameContainers.size()) break; // Done

        // Group all containers with the same result as this leader
        thisGroup.push_back(leaderIdx);
        handled[leaderIdx] = true;
        --left;
        for (size_t i = leaderIdx + 1; i < gameContainers.size(); ++i) {
            if (!handled[i] && gameContainers[leaderIdx].isSameResult(gameContainers[i])) {
                thisGroup.push_back(i);
                handled[i] = true;
                --left;
            }
        }

        // 5th line: comma separated manager names (implement/getGameManagerName() as needed)
        for (size_t j = 0; j < thisGroup.size(); ++j) {
            outStream << gameContainers[thisGroup[j]].getGameManagerName();
            if (j + 1 != thisGroup.size())
                outStream << ",";
        }
        outStream << "\n";

        // 6th line: game result message
        const GameResult& res = gameContainers[thisGroup[0]].getGameResult();

        outStream << gameResultMessage(res) << "\n";

        // 7th line: round number
        outStream << res.rounds << "\n";

        // 8th+: final map
        if (res.gameState) {
            for (const auto& row : getMapAsStrings(*res.gameState, gameContainers[0].getMapWidth(), gameContainers[0].getMapHeight())){
                outStream << row << "\n";
            }
        }

        // Separate groups with a blank line if there are more groups
        if (left > 0)
            outStream << "\n";
    }
    outStream.flush();
}

