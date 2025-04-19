#include "game/GameBoard.h"
#include <iostream>


void GameBoard::placeObject(const GameObjectPtr& obj) {
    Position pos = wrap(obj->getPosition());
    grid[pos.y][pos.x] = obj;
}

GameObjectPtr GameBoard::getObjectAt(Position p) const {
    Position wrapped = wrap(p);
    return grid[wrapped.y][wrapped.x];
}

void GameBoard::removeObjectAt(Position p) {
    Position wrapped = wrap(p);
    grid[wrapped.y][wrapped.x] = nullptr;
}

void GameBoard::printBoard() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x])
                std::cout << grid[y][x]->getSymbol();
            else
                std::cout << ' ';
        }
        std::cout << '\n';
    }
}


// TUNNEL wrap-around behavior
Position GameBoard::wrap(Position p) const {
    int x = ((p.x % width) + width) % width;
    int y = ((p.y % height) + height) % height;
    return Position(x, y);
}
