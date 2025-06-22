#include "GameObject.h"
namespace GameManager_322213836_212054837 {
// Constructor
    GameObject::GameObject(Position p, char s)
            : pos(p), symbol(s) {}


// Setter
    void GameObject::setPosition(Position p) {
        pos = p;
    }

    char GameObject::getSymbol() {
        return this->symbol;
    }
}