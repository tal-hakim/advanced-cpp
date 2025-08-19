#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "../../UserCommon/utils/Position.h"
#include "../../UserCommon/utils/definitions.h"
#include <climits>

namespace GameManager_322213836_212054837 {
    using namespace UserCommon_322213836_212054837;

    class GameObject {
    protected:
        Position pos;
        char symbol;
        bool destroyed = false;

    public:
        GameObject(Position p, char s);

        virtual ~GameObject() = default;

        Position getPosition() const { return pos; }

        void setPosition(Position p);

        virtual void destroy() { destroyed = true; }

        bool isDestroyed() const { return destroyed; }

        virtual char getSymbol();

        virtual std::string toString() const = 0;

    };
}
#endif // GAME_OBJECT_H
