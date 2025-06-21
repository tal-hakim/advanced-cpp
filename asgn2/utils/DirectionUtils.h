#pragma once

#include "Direction.h"
#include "Position.h"

namespace DirectionUtils {
    Position dirToVector(Direction dir);
    Direction directionFromTo(const Position& from, const Position& to);
    Direction getOppositeDirection(Direction dir);
}