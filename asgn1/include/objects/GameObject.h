#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "game/Position.h"
#include "definitions.h"

class GameObject {
protected:
    Position pos;
    char symbol;

public:
    GameObject(Position p, char s);
    virtual ~GameObject() = default;
    Position getPosition() const {return pos;}
    void setPosition(Position p);

    char getSymbol();
};

#endif // GAME_OBJECT_H
