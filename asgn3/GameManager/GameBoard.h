#ifndef ASGN3_GAMEBOARD_H
#define ASGN3_GAMEBOARD_H

#include "objects/GameObject.h"
#include "objects/MovingElement.h"
#include "objects/Wall.h"
#include "objects/Mine.h"
#include "objects/Shell.h"
#include "objects/Tank.h"
#include "../UserCommon/utils/Position.h"
#include "../Common/SatelliteView.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace GameManager_322213836_212054837 {

using GameObjectPtr = std::shared_ptr<GameObject>;
using GameGrid = std::vector<std::vector<std::vector<GameObjectPtr>>>;

class GameBoard {
private:
    size_t width;
    size_t height;
    GameGrid grid;

public:
    GameBoard() : width(0), height(0) {}
    GameBoard(size_t w, size_t h)
        : width(w),
          height(h),
          grid(w, std::vector<std::vector<GameObjectPtr>>(h)) {}

    Position wrap(Position p) const;

    void placeObject(const GameObjectPtr& obj);
    std::vector<GameObjectPtr> getObjectsAt(Position p) const;
    void removeSpecificObject(const GameObjectPtr& obj);
    void moveObj(const std::shared_ptr<MovingElement>& elem);

    void printBoard() const;
    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }

    bool isWall(const Position &pos) const;
    bool isMine(const Position &pos) const;
    bool isTank(const Position &pos) const;
    bool isShell(const Position &pos) const;

    std::vector<std::vector<char>> getBoardMat() const;
};

} // namespace GameManager

#endif // ASGN3_GAMEBOARD_H 