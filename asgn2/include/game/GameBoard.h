#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "objects/GameObject.h"
#include "objects/MovingElement.h"
#include "objects/Wall.h"
#include "objects/Mine.h"
#include "objects/Shell.h"
#include "objects/Tank.h"
#include "game/Position.h"
#include <vector>
#include <memory>
#include <algorithm> // for std::remove_if

using GameObjectPtr = std::shared_ptr<GameObject>;
using GameGrid = std::vector<std::vector<std::vector<GameObjectPtr>>>;

class GameBoard {
private:
    int width;
    int height;
    GameGrid grid;

public:
    GameBoard(int w, int h)
            : width(w),
              height(h),
              grid(h, std::vector<std::vector<GameObjectPtr>>(w)) {}

    Position wrap(Position p) const;

    void placeObject(const GameObjectPtr& obj);
    std::vector<GameObjectPtr> getObjectsAt(Position p) const;
    void removeSpecificObject(const GameObjectPtr& obj);
    void moveObj(const std::shared_ptr<MovingElement>& elem);

    void printBoard() const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool isWall(const Position &pos) const;

    bool isMine(const Position &pos) const;

    bool isTank(const Position &pos) const;

    bool isShell(const Position &pos) const;
};

#endif // GAME_BOARD_H
