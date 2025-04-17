#ifndef TANK_H
#define TANK_H

#include "MovingElement.h"
#include "game/Direction.h"
#include "game/Action.h"

class Tank : public MovingElement {
protected:
    int shellsRemaining = 16;
    int shootCooldown = 0;
    bool goingBack = false;
    int backwardTimer = 3;
    int playerId;

public:
    Tank(Position p, Direction dir, int id): MovingElement(p, '0' + id , dir){};

    void shoot();
    void rotate(int angle);

    int getShellsLeft() const{
        return shellsRemaining;
    };
    bool isGoingBack () const {return goingBack;}
    void setForward() {goingBack = false;}
    void setBackwards() {goingBack = true;}
    int getBackwardTimer() const {return  backwardTimer;}
    void setBackwardTimer(int newTimer){backwardTimer = newTimer;}
    int getPlayerId() const{return playerId;}
};

#endif // TANK_H
