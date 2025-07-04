//
// Created by talta on 24/06/2025.
//

#ifndef SIMULATOR_GAMECONTAINER_H
#define SIMULATOR_GAMECONTAINER_H
#include "GameManagerRegistrar.h"
#include "AlgorithmRegistrar.h"
#include "BoardInitInfo.h"
#include "../UserCommon/utils/definitions.h"
#include <map>
#include <iostream>

using std::string;


class GameContainer {
    BoardInitInfo& initInfo;
    std::unique_ptr<AbstractGameManager> gameManager;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    const TankAlgorithmFactory& player1AlgoFactory;
    const TankAlgorithmFactory& player2AlgoFactory;
    GameResult gameResult;
    const std::string& alg1Name;
    const std::string& alg2Name;
    const std::string& gameManagerName;

public:
    GameContainer(BoardInitInfo& initInfo,
                  std::unique_ptr<AbstractGameManager> gm,
                  std::unique_ptr<Player> p1,
                  std::unique_ptr<Player> p2,
                  const TankAlgorithmFactory& f1,
                  const TankAlgorithmFactory& f2,
                  const std::string& alg1Name, const std::string& alg2Name, const std::string& gameManagerName)
            : initInfo(initInfo),
              gameManager(std::move(gm)),
              player1(std::move(p1)),
              player2(std::move(p2)),
              player1AlgoFactory(f1),
              player2AlgoFactory(f2),
              alg1Name(alg1Name),
              alg2Name(alg2Name),
              gameManagerName(gameManagerName) {}

    void startGame() {
        // Note: we dereference the unique_ptr to get the SatelliteView
        if(!checkGameValidity()){
            return;
        }
        gameResult = gameManager->run(
                initInfo.width,
                initInfo.height,
                *(initInfo.satelliteView),
                initInfo.mapFilename,// Assuming SatelliteView is the base of BoardSatelliteView
                initInfo.maxSteps,
                initInfo.numShells,
                *player1, alg1Name,
                *player2, alg2Name,
                player1AlgoFactory,
                player2AlgoFactory
        );
        std::cout << alg1Name << " vs " << alg2Name << " in map: " << initInfo.mapFilename << std::endl;
        std::cout << "Winner: " << gameResult.winner << std::endl;
        std::cout << " ====================== " << std::endl;
    }


    bool checkGameValidity();

    void setGameResult(GameResult result){
        gameResult = std::move(result);
    }

    GameResult getGameResult(){
        return std::move(gameResult);
    }

    std::map<std::string, int> getGameScore() const;

    bool isSameResult(const GameContainer &other) const;

    string getAlg1Name(){
        return alg1Name;
    }

    string getAlg2Name(){
        return alg2Name;
    }

    string getGameManagerName(){
        return gameManagerName;
    }

    int getMapWidth() const{
        return initInfo.width;
    }

    int getMapHeight() const{
        return initInfo.height;
    }
};


#endif //SIMULATOR_GAMECONTAINER_H
