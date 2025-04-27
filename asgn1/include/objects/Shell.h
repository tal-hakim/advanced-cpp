#ifndef SHELL_H
#define SHELL_H

#include "MovingElement.h"

class Shell : public MovingElement {

public:
    Shell(Position p, Direction d, int playerId) : MovingElement(p, '*', d, playerId){};
    std::string toString() const override {
        return "Shell";
    }

};

#endif // SHELL_H
