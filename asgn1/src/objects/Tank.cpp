#include "objects/Tank.h"

void Tank::shoot() {
    shellsRemaining--;
    shootCooldown = SHOOTING_COOLDOWN;
}

bool Tank::canShoot() const {
    return shootCooldown < 0 && shellsRemaining > 0;
}

Direction Tank::getNewDir(int angle){
    int raw = static_cast<int>(dir);
    raw = (raw + angle + 8) % 8;
    return static_cast<Direction>(raw);
}
}

void Tank::rotate(int angle) {
    dir = getNewDir(angle);
}
