#include "objects/Tank.h"

void Tank::shoot() {
    shellsRemaining--;
    shootCooldown = 4;
}

void Tank::rotate(int angle) {
    int raw = static_cast<int>(dir);
    raw = (raw + angle + 8) % 8;
    dir = static_cast<Direction>(raw);
}
