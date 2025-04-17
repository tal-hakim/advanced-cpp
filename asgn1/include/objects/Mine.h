#ifndef MINE_H
#define MINE_H

#include "GameObject.h"

class Mine : public GameObject {
public:
    Mine(Position p): GameObject(p, '@'){};

};

#endif // MINE_H
