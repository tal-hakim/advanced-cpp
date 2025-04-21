#include "objects/Tank.h"

void Tank::shoot() {
    shellsRemaining--;
    shootCooldown = SHOOTING_COOLDOWN;
}

bool Tank::canShoot() const {
    return shootCooldown < 0 && shellsRemaining > 0;
}

void Tank::rotate(int angle) {
    int raw = static_cast<int>(dir);
    raw = (raw + angle + 8) % 8;
    dir = static_cast<Direction>(raw);
}
