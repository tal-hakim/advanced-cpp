#ifndef ASGN3_MOVINGELEMENT_H
#define ASGN3_MOVINGELEMENT_H

#include "GameObject.h"
#include "../../UserCommon/utils/Position.h"
#include "../../UserCommon/utils/DirectionUtils.h"

namespace GameManager_322213836_212054837 {
    using namespace UserCommon_322213836_212054837;

    class GameManager;  // Forward declaration

    class MovingElement : public GameObject {
        friend class GameManager;  // Allow GameManager to access protected members
    protected:
        Direction dir;
        int playerId;
        Position prevPos;

        void setPrevPos() { prevPos = pos; }

        Position getPrevPos() const { return prevPos; }

    public:
        MovingElement(Position p, char symbol, Direction dir, int playerId) : GameObject(p, symbol),
                                                                              dir(dir), playerId(playerId) {};

        int getPlayerId() const { return playerId; }

        Direction getDirection() const { return dir; }

        void setDirection(Direction newDir) { dir = newDir; }

        Position getNextPos(bool backwards = false) const;
    };
}
#endif // ASGN3_MOVINGELEMENT_H 