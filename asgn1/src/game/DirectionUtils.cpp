#include "DirectionUtils.h"

namespace DirectionUtils {

Position dirToVector(Direction dir) {
    switch (dir) {
        case Direction::U:  return {0, -1};
        case Direction::UR: return {1, -1};
        case Direction::R:  return {1, 0};
        case Direction::DR: return {1, 1};
        case Direction::D:  return {0, 1};
        case Direction::DL: return {-1, 1};
        case Direction::L:  return {-1, 0};
        case Direction::UL: return {-1, -1};
    }
    return {0, 0}; // fallback
}

Direction directionFromTo(const Position& from, const Position& to) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    dx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    dy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    if (dx == 0 && dy == -1) return Direction::U;
    if (dx == 1 && dy == -1) return Direction::UR;
    if (dx == 1 && dy == 0)  return Direction::R;
    if (dx == 1 && dy == 1)  return Direction::DR;
    if (dx == 0 && dy == 1)  return Direction::D;
    if (dx == -1 && dy == 1) return Direction::DL;
    if (dx == -1 && dy == 0) return Direction::L;
    if (dx == -1 && dy == -1)return Direction::UL;

    return Direction::U;  // default/fallback
}

} // namespace DirectionUtils
