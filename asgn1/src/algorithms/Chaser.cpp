#include "algorithms/Chaser.h"
#include "objects/Shell.h"
#include "game/Direction.h"
#include "game/Action.h"
#include "game/DirectionUtils.h"
#include <queue>
#include <unordered_set>
#include <string>

Action Chaser::getNextAction(const GameBoard& board,
                             const Tank& myTank,
                             const Tank& opponentTank)
{
    auto bfsToEnemy = [&](const Position& start, const Position& goal) {
        std::queue<std::pair<Position, std::vector<Position>>> q;
        std::unordered_set<std::string> visited;

        q.push({start, {start}});
        visited.insert(start.toString());

        while (!q.empty()) {
            auto [current, path] = q.front(); q.pop();

            if (current == goal) return path;

            for (int d = 0; d < 8; ++d) {
                Direction dir = static_cast<Direction>(d);
                Position next = board.wrap(current + DirectionUtils::dirToVector(dir));

                // Skip walls, mines, and previously visited positions
                if (!board.isWall(next) &&
                    !board.isMine(next) &&
                    visited.count(next.toString()) == 0) {

                    visited.insert(next.toString());
                    auto newPath = path;
                    newPath.push_back(next);
                    q.push({next, newPath});
                }
            }
        }

        return std::vector<Position>{};
    };

    auto pathBlocked = [&](const std::vector<Position>& path) {
        for (const auto& pos : path) {
            if (board.isWall(pos) || board.isMine(pos))
                return true;  // Blocked if wall or mine is encountered
        }
        return false;
    };

    bool shouldRecalculate = bfsPath.empty() ||
                             lastEnemyPos != opponentTank.getPosition() ||
                             pathBlocked(bfsPath);

    if (shouldRecalculate) {
        bfsPath = bfsToEnemy(myTank.getPosition(), opponentTank.getPosition());
        lastEnemyPos = opponentTank.getPosition();
    }

    if (bfsPath.size() < 2)
        return rotateToward(myTank.getDirection(), DirectionUtils::directionFromTo(myTank.getPosition(), opponentTank.getPosition()));

    auto canShootEnemy = [&]() -> bool {
        Position myPos = myTank.getPosition();
        Direction myDir = myTank.getDirection();
        int steps = stepsUntilShellHitsTank(Shell(myPos, myDir, myTank.getPlayerId()), opponentTank, board);
        return myTank.canShoot() && steps >=0;
    };

    // Check if we should fire at the enemy
    if (canShootEnemy())
        return Action::Shoot;

    Position next = bfsPath[1];
    std::cout << "chaser is going to: " << next << std::endl;
    Direction desiredDir = DirectionUtils::directionFromTo(myTank.getPosition(), next);
    std::cout << "chaser dir needs to be: " << static_cast<int>(desiredDir) << std::endl;
    int angleDiff = static_cast<int>(desiredDir) - static_cast<int>(myTank.getDirection());
    if (angleDiff < 0) angleDiff += 8;  // Normalize to positive difference

    // Rotate clockwise (right) 1/8 at a time
//    Direction newDir = static_cast<Direction>((static_cast<int>(myTank.getDirection()) + 1) % 8);
//    return rotateToward(myTank.getDirection(), newDir);

    // Rotate if not facing the desired direction
    if (myTank.getDirection() != desiredDir)
        return rotateToward(myTank.getDirection(), desiredDir);
    bfsPath.erase(bfsPath.begin());
    return Action::MoveFwd;
}


