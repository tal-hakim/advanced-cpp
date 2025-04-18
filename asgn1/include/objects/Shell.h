#ifndef SHELL_H
#define SHELL_H

#include "MovingElement.h"

class Shell : public MovingElement {

public:
    Shell(Position p, Direction d, int playerId) : MovingElement(p, ' ', d, playerId){};

};

#endif // SHELL_H
