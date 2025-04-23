#include "objects/MovingElement.h"
#include "game/DirectionUtils.h"

Position MovingElement::getNextPos(bool backwards) const {
    Position current = getPosition();
    Position delta = DirectionUtils::dirToVector(dir);
    if (backwards){
        delta = -delta;
    }
    return current + delta;

}
