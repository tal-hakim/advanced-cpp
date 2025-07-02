#include "DirectionUtils.h"
#include "definitions.h"

namespace UserCommon_322213836_212054837 {
    namespace DirectionUtils {

        Position dirToVector(Direction dir) {
            switch (dir) {
                case Direction::U:
                    return {0, -1};
                case Direction::UR:
                    return {1, -1};
                case Direction::R:
                    return {1, 0};
                case Direction::DR:
                    return {1, 1};
                case Direction::D:
                    return {0, 1};
                case Direction::DL:
                    return {-1, 1};
                case Direction::L:
                    return {-1, 0};
                case Direction::UL:
                    return {-1, -1};
            }
            return {0, 0}; // fallback
        }

        Direction directionFromTo(const Position &from, const Position &to) {
            int dist_x = to.x - from.x;
            int dist_y = to.y - from.y;

            int dx = (dist_x > 0) ? 1 : (dist_x < 0) ? -1 : 0;
            int dy = (dist_y > 0) ? 1 : (dist_y < 0) ? -1 : 0;

            if (abs(dist_x) > 1) {
                dx = -dx;
            }

            if (abs(dist_y) > 1) {
                dy = -dy;
            }

            if (dx == 0 && dy == -1) return Direction::U;
            if (dx == 1 && dy == -1) return Direction::UR;
            if (dx == 1 && dy == 0) return Direction::R;
            if (dx == 1 && dy == 1) return Direction::DR;
            if (dx == 0 && dy == 1) return Direction::D;
            if (dx == -1 && dy == 1) return Direction::DL;
            if (dx == -1 && dy == 0) return Direction::L;
            if (dx == -1 && dy == -1)return Direction::UL;

            return Direction::U;  // default/fallback
        }

        Direction getOppositeDirection(Direction dir) {
            return static_cast<Direction>((static_cast<int>(dir) + 4) % NUM_DIRECTIONS);
        }

    } // namespace DirectionUtils
} // namespace UserCommon_322213836_212054837