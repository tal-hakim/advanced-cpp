#ifndef MINE_H
#define MINE_H

#include "objects/GameObject.h"
#include "definitions.h"

class Mine : public GameObject {
public:
    Mine(Position p): GameObject(p, MINE){};
    std::string toString() const override {
        return "Mine";
    }

};

#endif // MINE_H
