#include "game/GameBoard.h"
#include <iostream>
#include <algorithm>

void GameBoard::placeObject(const GameObjectPtr& obj) {
    Position pos = wrap(obj->getPosition());
    grid[pos.y][pos.x].push_back(obj);
}

std::vector<GameObjectPtr> GameBoard::getObjectsAt(Position p) const {
    Position wrapped = wrap(p);
    return grid[wrapped.y][wrapped.x];
}


void GameBoard::removeSpecificObject(const GameObjectPtr& obj) {
    Position pos = wrap(obj->getPosition());
    auto& cell = grid[pos.y][pos.x];
    cell.erase(std::remove(cell.begin(), cell.end(), obj), cell.end());
}

void GameBoard::moveObj(const std::shared_ptr<MovingElement>& elem) {
    removeSpecificObject(elem);
    placeObject(elem);
}

void GameBoard::printBoard() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!grid[y][x].empty())
                std::cout << grid[y][x].back()->getSymbol();  // draw top-most object
            else
                std::cout << ' ';
        }
        std::cout << '\n';
    }
}

Position GameBoard::wrap(Position p) const {
    int x = ((p.x % width) + width) % width;
    int y = ((p.y % height) + height) % height;
    return Position(x, y);
}
