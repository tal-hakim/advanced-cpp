
#ifndef MOVINGELEMENT_H
#define MOVINGELEMENT_H

#include "objects/GameObject.h"
#include "game/Direction.h"
#include "game/DirectionUtils.h"

class MovingElement : public GameObject {
protected:
    Direction dir;
    int playerId;
    Position prevPos;

public:
    MovingElement(Position p, char symbol, Direction dir, int playerId): GameObject(p, symbol),
        dir(dir), playerId(playerId) {};
    int getPlayerId() const{return playerId;}
    Direction getDirection() const {return dir;}
    void setDirection(Direction newDir){dir = newDir;}
    Position getNextPos(bool backwards = false) const;
    void setPrevPos() { prevPos = pos; }
    Position getPrevPos() const { return prevPos; }

};

#endif //MOVINGELEMENT_H
