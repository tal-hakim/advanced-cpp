#ifndef ASGN3_POSITION_H
#define ASGN3_POSITION_H

#include <string>
#include <iostream>

namespace UserCommon_322213836_212054837 {

    struct Position {
        int x;
        int y;

        Position(int x = 0, int y = 0) : x(x), y(y) {}

        Position(size_t x, size_t y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}
        // Equality comparison
        bool operator==(const Position &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Position &other) const {
            return !(*this == other);
        }

        // Addition of two positions
        Position operator+(const Position &other) const {
            return Position(x + other.x, y + other.y);
        }

        // Subtraction of two positions
        Position operator-(const Position &other) const {
            return Position(x - other.x, y - other.y);
        }

        // Debug print
        friend std::ostream &operator<<(std::ostream &os, const Position &pos) {
            os << "(" << pos.x << ", " << pos.y << ")";
            return os;
        }

        std::string toString() const {
            return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
        }


        inline Position operator-() {
            return {-x, -y};
        }


    };
}  // namespace UserCommon_322213836_212054837
namespace std {
        template<>
        struct hash<UserCommon_322213836_212054837::Position> {
            size_t operator()(const UserCommon_322213836_212054837::Position &p) const {
                return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
            }
        };
    }



#endif // ASGN3_POSITION_H 