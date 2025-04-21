#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "game/Position.h"
#include "game/GameBoard.h"
#include "definitions.h"


class GameObject {
protected:
    Position pos;
    char symbol;
    bool destroyed = false;

public:
    GameObject(Position p, char s);
    virtual ~GameObject() = default;
    Position getPosition() const {return pos;}
    void setPosition(Position p);
    virtual void destroy(GameBoard& board) { destroyed = true; }
    bool isDestroyed() const{ return destroyed; }
    virtual char getSymbol();
    virtual std::string toString() const = 0;

#endif // GAME_OBJECT_H
