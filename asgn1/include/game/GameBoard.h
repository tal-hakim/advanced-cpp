#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "objects/GameObject.h"
#include "objects/MovingElement.h"
#include "game/Position.h"
#include <vector>
#include <memory>

using GameObjectPtr = std::shared_ptr<GameObject>;
using GameGrid = std::vector<std::vector<GameObjectPtr>>;

class GameBoard {
private:
    int width;
    int height;
    GameGrid grid;



public:
    GameBoard(int w, int h) : width(w), height(h), grid(h, std::vector<GameObjectPtr>(w, nullptr)) {};
    Position wrap(Position p) const;
    void placeObject(const GameObjectPtr& obj);
    GameObjectPtr getObjectAt(Position p) const;
    void removeObjectAt(Position p);
    void moveObj(const std::shared_ptr<MovingElement>& elem) {removeObjectAt(elem->getPrevPos()); placeObject(elem);}
    void printBoard() const;
    int getWidth() const {return width;}
    int getHeight() const {return height;}
};

#endif // GAME_BOARD_H
