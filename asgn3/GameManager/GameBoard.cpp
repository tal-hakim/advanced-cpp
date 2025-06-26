#include "GameBoard.h"
#include "../UserCommon/utils/definitions.h"
#include <iostream>
#include <algorithm>

namespace GameManager_322213836_212054837 {

void GameBoard::placeObject(const GameObjectPtr& obj) {
    Position pos = wrap(obj->getPosition());
    grid[pos.x][pos.y].push_back(obj);
}

std::vector<GameObjectPtr> GameBoard::getObjectsAt(Position p) const {
    Position wrapped = wrap(p);
    return grid[wrapped.x][wrapped.y];
}

void GameBoard::removeSpecificObject(const GameObjectPtr& obj) {
    Position pos = wrap(obj->getPosition());
    auto& cell = grid[pos.x][pos.y];
    cell.erase(std::remove(cell.begin(), cell.end(), obj), cell.end());
}

void GameBoard::moveObj(const std::shared_ptr<MovingElement>& elem) {
    removeSpecificObject(elem);
    placeObject(elem);
}

void GameBoard::printBoard() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!grid[x][y].empty())
                std::cout << grid[x][y].back()->getSymbol();  // print top-most object
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}

Position GameBoard::wrap(Position p) const {
    int x = ((p.x % width) + width) % width;
    int y = ((p.y % height) + height) % height;
    return Position(x, y);
}

bool GameBoard::isWall(const Position& pos) const {
    Position wrapped = wrap(pos);
    const auto& objects = getObjectsAt(wrapped);

    for (const auto& obj : objects) {
        if (obj && std::dynamic_pointer_cast<Wall>(obj)) {
            return true;
        }
    }
    return false;
}

bool GameBoard::isMine(const Position& pos) const {
    Position wrapped = wrap(pos);
    const auto& objects = getObjectsAt(wrapped);

    for (const auto& obj : objects) {
        if (obj && std::dynamic_pointer_cast<Mine>(obj)) {
            return true;
        }
    }
    return false;
}

bool GameBoard::isShell(const Position& pos) const {
    Position wrapped = wrap(pos);
    const auto& objects = getObjectsAt(wrapped);

    for (const auto& obj : objects) {
        if (obj && std::dynamic_pointer_cast<Shell>(obj)) {
            return true;
        }
    }
    return false;
}

bool GameBoard::isTank(const Position& pos) const {
    Position wrapped = wrap(pos);
    const auto& objects = getObjectsAt(wrapped);

    for (const auto& obj : objects) {
        if (obj && std::dynamic_pointer_cast<Tank>(obj)) {
            return true;
        }
    }
    return false;
}

std::vector<std::vector<char>> GameBoard::getBoardMat() const {
    std::vector<std::vector<char>> mat(width, std::vector<char>(height, EMPTY));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const auto& cell = grid[x][y];
            if (!cell.empty()) {
                mat[x][y] = cell.back()->getSymbol();  // draw top-most object
            }
        }
    }

    return mat;
}

} // namespace GameManager