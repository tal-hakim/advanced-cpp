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
using std::string;


class GameContainer {
    const BoardInitInfo& initInfo;
    std::unique_ptr<AbstractGameManager> gameManager;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    const TankAlgorithmFactory& player1AlgoFactory = nullptr;
    const TankAlgorithmFactory& player2AlgoFactory = nullptr;
    GameResult gameResult;
    const std::string& alg1Name;
    const std::string& alg2Name;
    const std::string& gameManagerName;

public:
    GameContainer(const BoardInitInfo& initInfo,
                  std::unique_ptr<AbstractGameManager> gm,
                  std::unique_ptr<Player> p1,
                  std::unique_ptr<Player> p2,
                  const TankAlgorithmFactory& f1,
                  const TankAlgorithmFactory& f2,
                  const std::string& alg1Name, const std::string& alg2Name, const std::string& gameManagerName)
            : initInfo(std::move(initInfo)),
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
        gameResult = gameManager->run(
                initInfo.width,
                initInfo.height,
                *(initInfo.satelliteView), // Assuming SatelliteView is the base of BoardSatelliteView
                initInfo.maxSteps,
                initInfo.numShells,
                *player1,
                *player2,
                player1AlgoFactory,
                player2AlgoFactory
        );
    }



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
