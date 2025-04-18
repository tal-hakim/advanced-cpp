
#ifndef MOVINGELEMENT_H

#include "GameObject.h"
#include "game/Direction.h"

class MovingElement : public GameObject {
protected:
    Direction dir;
    int playerId;
    Position prevPos;

public:
    MovingElement(Position p, char symbol, Direction dir, int playerId): GameObject(p, symbol),
        dir(dir), playerId(playerId){};

    Direction getDirection() const {return dir;}
    void setDirection(Direction newDir){dir = newDir;}
    Position getNextPos(bool backwards = false) const;
    void setPrevPos() { prevPos = pos; }
    Position getPrevPos() const { return prevPos; }
};
#define MOVINGELEMENT_H

#endif //MOVINGELEMENT_H
