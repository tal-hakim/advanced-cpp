#ifndef TANK_H
#define TANK_H

#include "MovingElement.h"
#include "game/Direction.h"
#include "game/Action.h"


class Tank : public MovingElement {
protected:
    int shellsRemaining = INITIAL_SHELLS_AMOUNT;
    int shootCooldown = 0;
    bool goingBack = false;
    int backwardTimer = UNINITIALIZED;
    int lastBackwardStep = INT_MIN;

public:
    Tank(Position p, Direction dir, int id): MovingElement(p, '0' + id , dir, id){};

    void shoot();
    void rotate(int angle);

    int getShellsLeft() const{
        return shellsRemaining;
    };
    bool isGoingBack () const {return backwardTimer != UNINITIALIZED;}
    void setForward() {backwardTimer = UNINITIALIZED;}
    void setBackwards() { backwardTimer = BACKWARDS_STEP_COUNT;}
    int getBackwardTimer() const {return  backwardTimer;}
    void decreaseShootCooldown() { shootCooldown--; }
    void decreaseBackwardTimer() { if(backwardTimer > 0) backwardTimer--; }
    int getPlayerId() const{return playerId;}
    bool canShoot();
    bool isOutOfAmmo() { return shellsRemaining == 0; }
    void setLastBackwardStep(int step) { lastBackwardStep = step; }
    bool isLastStepBack(int step) { return step - lastBackwardStep == 2; }
};

#endif // TANK_H
