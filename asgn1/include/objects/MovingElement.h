
#ifndef MOVINGELEMENT_H

#include "GameObject.h"
#include "game/Direction.h"

class MovingElement : public GameObject {
protected:
    Direction dir;
public:
    MovingElement(Position p, char symbol, Direction dir): GameObject(p, symbol), dir(dir){};
    Direction getDirection() const {return dir;}
    void setDirection(Direction newDir){dir = newDir;}
    Position getNextPos(bool backwards = false) const;
};
#define MOVINGELEMENT_H

#endif //MOVINGELEMENT_H
