//
// Created by talta on 24/06/2025.
//

#include "CompetitiveSimulator.h"

void CompetitiveSimulator::buildGameContainers() {
    auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    // Find game manager entry (assuming you use the first one)
    assert(gmRegistrar.count() > 0 && "No game managers registered!");
    const auto& gmEntry = *gmRegistrar.begin();

    int N = algorithmsSONames.size();
    int K = boards.size();

    for (int k = 0; k < K; ++k) {
        for (int i = 0; i < N; ++i) {
            int j = (i + 1 + k % (N - 1)) % N;
            if (N % 2 == 0 && k % (N-1) == N/2 - 1 && i > j) continue; // avoid duplicate matches when N even

            auto& alg1Entry = *(algoRegistrar.begin() + i);
            auto& alg2Entry = *(algoRegistrar.begin() + j);

            const std::string& alg1Name = alg1Entry.name();
            const std::string& alg2Name = alg2Entry.name();

            auto player1 = alg1Entry.createPlayer(PLAYER_1_ID, boards[k].width, boards[k].height, boards[k].maxSteps, boards[k].numShells);
            auto player2 = alg2Entry.createPlayer(PLAYER_2_ID, boards[k].width, boards[k].height, boards[k].maxSteps, boards[k].numShells);

            // Create game manager
            auto gm = gmEntry.createGameManager(verbose);

            // Build GameContainer
            gameContainers.emplace_back(
                    boards[k],
                    std::move(gm),
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
}




void CompetitiveSimulator::logResults() {
    if (gameContainers.empty()) return;

    // Output file name (as required)
    std::string outFile = algorithmsFolder + "/competition_" + getCurrentTimeString() + ".txt";
    std::ofstream out(outFile);
    bool toScreen = false;
    if (!out) {
        std::cerr << "Failed to open output file '" << outFile << "'. Printing to screen instead.\n";
        toScreen = true;
    }
    std::ostream& outStream = toScreen ? std::cout : out;

    // 1st and 2nd lines
    outStream << "game_maps_folder=" << mapsFolder << "\n";
    outStream << "game_manager=" << gameContainers[0].getGameManagerName() << "\n\n";

    // Accumulate scores
    std::map<std::string, int> totalScores;
         // ensure all algorithms appear, even if they lost all games

    for (const auto& gc : gameContainers) {
        std::map<std::string, int> gameScore = gc.getGameScore();
        for (const auto& [alg, score] : gameScore)
            totalScores[alg] += score;
    }

    // Sort algorithms by score, descending
    std::vector<std::pair<std::string, int>> sortedScores(totalScores.begin(), totalScores.end());
    std::sort(sortedScores.begin(), sortedScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Output the results
    for (const auto& [alg, score] : sortedScores)
        outStream << alg << " " << score << "\n";
}

