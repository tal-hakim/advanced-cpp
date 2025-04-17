#ifndef SHELL_H
#define SHELL_H

#include "MovingElement.h"

class Shell : public MovingElement {

public:
    Shell(Position p, Direction d) : MovingElement(p, ' ', d){};

};

#endif // SHELL_H
