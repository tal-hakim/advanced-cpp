#ifndef MINE_H
#define MINE_H

#include "GameObject.h"
#include "../UserCommon/utils/definitions.h"
namespace GameManager_322213836_212054837 {
    class Mine : public GameObject {
    public:
        Mine(Position p) : GameObject(p, MINE) {};

        std::string toString() const override {
            return "Mine";
        }

    };
}
#endif // MINE_H
