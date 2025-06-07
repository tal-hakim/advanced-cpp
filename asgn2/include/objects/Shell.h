#ifndef SHELL_H
#define SHELL_H

#include "objects//MovingElement.h"
#include "definitions.h"

class Shell : public MovingElement {

public:
    Shell(Position p, Direction d, int playerId) : MovingElement(p, SHELL, d, playerId){};
    std::string toString() const override {
        return "Shell";
    }

};

#endif // SHELL_H
