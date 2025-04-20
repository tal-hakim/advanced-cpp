#ifndef WALL_H
#define WALL_H

#include "GameObject.h"

class Wall : public GameObject {
private:
    int hitCount = 0;

public:
    Wall(Position p) : GameObject(p, '#') {}
    void takeHit() {hitCount++;}
    bool isDestroyed() const {return hitCount >= 2;}
    void destroy() { takeHit(); if(isDestroyed()) {GameObject::destroy();} }
};

#endif // WALL_H
