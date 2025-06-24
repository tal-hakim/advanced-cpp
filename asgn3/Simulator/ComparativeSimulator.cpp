//
// Created by talta on 24/06/2025.
//

#include "ComparativeSimulator.h"

void ComparativeSimulator::buildGameContainers() {
    auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    const BoardInitInfo& board = boards[0];

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
                alg2Name
        );
    }
}

