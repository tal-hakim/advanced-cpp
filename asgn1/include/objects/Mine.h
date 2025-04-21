#ifndef MINE_H
#define MINE_H

#include "GameObject.h"

class Mine : public GameObject {
public:
    Mine(Position p): GameObject(p, '@'){};
    std::string toString() const override {
        return "Mine";
    }

};

#endif // MINE_H
