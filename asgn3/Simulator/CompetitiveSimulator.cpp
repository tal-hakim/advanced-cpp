//
// Created by talta on 24/06/2025.
//

#include "CompetitiveSimulator.h"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>
#include "GameContainer.h"

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
            if (N % 2 == 0 && k == N/2 - 1 && i > j) continue; // avoid duplicate matches when N even

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
                    alg2Name
            );
        }
    }
}




void CompetitiveSimulator::logResults() {

}
