#include "objects/MovingElement.h"

Position MovingElement::getNextPos(bool backwards) const {
    Position current = getPosition();
    Position delta = DirectionUtils::dirToVector(dir);
    if (backwards){
        delta = -delta;
    }
    return current + delta;

}
