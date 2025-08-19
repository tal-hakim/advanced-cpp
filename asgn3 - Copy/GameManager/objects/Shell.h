#ifndef ASGN3_SHELL_H
#define ASGN3_SHELL_H

#include "MovingElement.h"

namespace GameManager_322213836_212054837 {

    class Shell : public MovingElement {

    public:
        Shell(Position p, Direction d, int playerId) : MovingElement(p, SHELL, d, playerId){};
        std::string toString() const override {
            return "Shell";
        }

    };


} // GameManager

#endif // ASGN3_SHELL_H 