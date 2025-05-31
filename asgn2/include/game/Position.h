#ifndef POSITION_H
#define POSITION_H

#include <iostream>

struct Position {
    int x;
    int y;

    Position(int x = 0, int y = 0) : x(x), y(y) {}

    // Equality comparison
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    // Addition of two positions
    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y);
    }

    // Subtraction of two positions
    Position operator-(const Position& other) const {
        return Position(x - other.x, y - other.y);
    }

    // Debug print
    friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        os << "(" << pos.x << ", " << pos.y << ")";
        return os;
    }

    std::string toString() const {
        return "(" + std::to_string(x) + "," + std::to_string(y)+")";
    }


    inline Position operator-() {
        return { -x, -y };
    }
    
    
};

namespace std {
    template<>
    struct hash<Position> {
        size_t operator()(const Position& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
}

#endif // POSITION_H
