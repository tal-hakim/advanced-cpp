#ifndef MOVINGELEMENT_H
#define MOVINGELEMENT_H

#include "objects/GameObject.h"
#include "utils/Direction.h"
#include "utils/DirectionUtils.h"

class GameManager;  // Forward declaration

class MovingElement : public GameObject {
    friend class GameManager;  // Allow GameManager to access protected members
protected:
    Direction dir;
    int playerId;
    Position prevPos;
    void setPrevPos() { prevPos = pos; }
    Position getPrevPos() const { return prevPos; }

public:
    MovingElement(Position p, char symbol, Direction dir, int playerId): GameObject(p, symbol),
        dir(dir), playerId(playerId) {};
    int getPlayerId() const{return playerId;}
    Direction getDirection() const {return dir;}
    void setDirection(Direction newDir){dir = newDir;}
    Position getNextPos(bool backwards = false) const;
};

#endif //MOVINGELEMENT_H
