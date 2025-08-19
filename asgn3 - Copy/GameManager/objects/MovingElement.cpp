#include "MovingElement.h"

namespace GameManager_322213836_212054837 {

    Position MovingElement::getNextPos(bool backwards) const {
        Position current = getPosition();
        Position delta = DirectionUtils::dirToVector(dir);
        if (backwards){
            delta = -delta;
        }
        return current + delta;

    }
}
