#include "objects/GameObject.h"

// Constructor
GameObject::GameObject(Position p, char s)
        : pos(p), symbol(s) {}

// Getter
Position GameObject::getPosition() const {
    return pos;
}

// Setter
void GameObject::setPosition(Position p) {
    pos = p;
}

char GameObject::getSymbol() {
    return this->symbol;
}
