#ifndef TANK_H
#define TANK_H

#include "MovingElement.h"
#include "game/Direction.h"
#include "game/Action.h"

class GameManager;  // Forward declaration

class Tank : public MovingElement {
    friend class GameManager;  // Allow GameManager to access protected members
protected:
    int shellsRemaining = INITIAL_SHELLS_AMOUNT;
    int shootCooldown = 0;
    int backwardTimer = UNINITIALIZED;
    int lastBackwardStep = INT_MIN;
    std::string collisionType = "";
    Direction getNewDir(int angle);
    void decreaseShootCooldown() { shootCooldown--; }
    void decreaseBackwardTimer() { if(backwardTimer > 0) backwardTimer--; }
    void setLastBackwardStep(int step) { lastBackwardStep = step; }
    bool isLastStepBack(int step) { return step - lastBackwardStep == 1; }

public:
    Tank(Position p, Direction dir, int id): MovingElement(p, '0' + id , dir, id){};

    void shoot();
    void rotate(int angle);

    bool isGoingBack () const {return backwardTimer != UNINITIALIZED;}
    void setForward() {backwardTimer = UNINITIALIZED;}
    void setBackwards() { backwardTimer = BACKWARDS_STEP_COUNT;}
    int getBackwardTimer() const {return  backwardTimer;}
    bool canShoot() const;
    bool isOutOfAmmo() { return shellsRemaining == 0; }
    std::string getCollisionType() const { return collisionType; }
    void setCollisionType(std::string colType) {if(collisionType == "") collisionType = colType;}
    std::string toString() const override {
        return "Tank "+ std::to_string(playerId);
    }

};

#endif // TANK_H
