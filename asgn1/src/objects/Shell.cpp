#include "objects/Shell.h"

void Shell::move() {
    Position current = getPosition();

    int dx = 0;
    int dy = 0;

    switch (dir) {
        case Direction::U:   dy = -1; break;
        case Direction::UR:  dx = 1; dy = -1; break;
        case Direction::R:   dx = 1; break;
        case Direction::DR:  dx = 1; dy = 1; break;
        case Direction::D:   dy = 1; break;
        case Direction::DL:  dx = -1; dy = 1; break;
        case Direction::L:   dx = -1; break;
        case Direction::UL:  dx = -1; dy = -1; break;
        default: break;
    }

    // Create updated position
    Position newPos = { current.x + dx, current.y + dy };
    setPosition(newPos); // Use setter ✅
}