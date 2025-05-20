#ifndef TANK_H
#define TANK_H

#include <memory>
#include "MovingElement.h"
#include "game/Direction.h"
#include "../../common/ActionRequest.h"
#include "../../common/TankAlgorithm.h"

class Tank : public MovingElement {
protected:
    int shellsRemaining = INITIAL_SHELLS_AMOUNT;
    int shootCooldown = 0;
    int backwardTimer = UNINITIALIZED;
    int lastBackwardStep = INT_MIN;
    std::string collisionType = "";
    std::unique_ptr<TankAlgorithm> algorithm;

public:
    Tank(Position p, Direction dir, int id, int numShells = INITIAL_SHELLS_AMOUNT)
        : MovingElement(p, '0' + id, dir, id), shellsRemaining(numShells) {}

    void shoot();
    void rotate(int angle);

    int getShellsLeft() const { return shellsRemaining; }
    bool isGoingBack() const { return backwardTimer != UNINITIALIZED; }
    void setForward() { backwardTimer = UNINITIALIZED; }
    void setBackwards() { backwardTimer = BACKWARDS_STEP_COUNT; }
    int getBackwardTimer() const { return backwardTimer; }
    void decreaseShootCooldown() { shootCooldown--; }
    void decreaseBackwardTimer() { if(backwardTimer > 0) backwardTimer--; }
    bool canShoot() const;
    bool isOutOfAmmo() { return shellsRemaining == 0; }
    void setLastBackwardStep(int step) { lastBackwardStep = step; }
    bool isLastStepBack(int step) { return step - lastBackwardStep == 1; }
    std::string getCollisionType() const { return collisionType; }
    void setCollisionType(std::string colType) { if(collisionType == "") collisionType = colType; }
    std::string toString() const override { return "Tank " + std::to_string(playerId); }
    int getCooldown() const { return shootCooldown; }
    Direction getNewDir(int angle);

    void setAlgorithm(std::unique_ptr<TankAlgorithm> algo) { algorithm = std::move(algo); }
    TankAlgorithm* getAlgorithm() { return algorithm.get(); }
};

#endif // TANK_H
