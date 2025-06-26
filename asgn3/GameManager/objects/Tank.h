#ifndef ASGN3_TANK_H
#define ASGN3_TANK_H

#include "MovingElement.h"
#include "../../Common/TankAlgorithm.h"
#include "../../UserCommon/utils/Position.h"
#include "../../UserCommon/utils/Direction.h"
#include <memory>


namespace GameManager_322213836_212054837 {
    using std::unique_ptr;
    using namespace UserCommon_322213836_212054837;

    class GameManager;  // Forward declaration

    class Tank : public MovingElement {
        friend class GameManager;  // Allow GameManager to access protected members
    protected:
        size_t shellsRemaining;
        int shootCooldown = 0;
        int backwardTimer = UNINITIALIZED;
        int lastBackwardStep = INT_MIN;
        std::string collisionType = "";
        std::unique_ptr<TankAlgorithm> algorithm;

        Direction getNewDir(int angle);

        int globalIndex;

    public:
        Tank(Position p, Direction dir, int id, int index, size_t numShells = INITIAL_SHELLS_AMOUNT)
                : MovingElement(p, '0' + id, dir, id), shellsRemaining(numShells), globalIndex(index) {}

        void shoot();

        void rotate(int angle);

        size_t getShellsLeft() const { return shellsRemaining; }

        bool isGoingBack() const { return backwardTimer != UNINITIALIZED; }

        void setForward() { backwardTimer = UNINITIALIZED; }

        void setBackwards() { backwardTimer = BACKWARDS_STEP_COUNT; }

        int getBackwardTimer() const { return backwardTimer; }

        void decreaseShootCooldown() { shootCooldown--; }

        void decreaseBackwardTimer() { if (backwardTimer > 0) backwardTimer--; }

        bool canShoot() const;

        bool isOutOfAmmo() { return shellsRemaining == 0; }

        void setLastBackwardStep(int step) { lastBackwardStep = step; }

        bool isLastStepBack(int step) { return step - lastBackwardStep == 1; }

        std::string getCollisionType() const { return collisionType; }

        void setCollisionType(std::string colType) { if (collisionType == "") collisionType = colType; }

        std::string toString() const override { return "Tank " + std::to_string(playerId); }

        int getCooldown() const { return shootCooldown; }

        void setAlgorithm(std::unique_ptr<TankAlgorithm> algo) { algorithm = std::move(algo); }

        TankAlgorithm *getAlgorithm() { return algorithm.get(); }

        int getGlobalIndex() const { return globalIndex; }
    };

}
#endif // ASGN3_TANK_H 