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

            for (int d = 0; d < NUM_DIRECTIONS ; ++d) {
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

    auto canShootEnemy = [&](Direction dir) -> bool {
        Position myPos = myTank.getPosition();
        int steps = stepsUntilShellHitsTank(Shell(myPos, dir, myTank.getPlayerId()), opponentTank, board);
        return myTank.canShoot() && steps >=0;
    };

    if (canShootEnemy(myTank.getDirection()))
        return Action::Shoot;

    if (bfsPath.size() < 7) {
//        return Action::Shoot;
        for (int i = LEFT_ANGLE_1_4; i < 6; i++) {
            int d = (static_cast<int>(myTank.getDirection()) + i + NUM_DIRECTIONS ) % NUM_DIRECTIONS ;
            Direction dir = static_cast<Direction>(d);
            if (canShootEnemy(dir)){
                if(i != 0){
                    return rotateToward(myTank.getDirection(),dir);
                }

            }
        }
    }


    if(bfsPath.size() < 4){
        return Action::Shoot;
    }
    Position next = bfsPath[1];
    Direction desiredDir = DirectionUtils::directionFromTo(myTank.getPosition(), next);
    int angleDiff = static_cast<int>(desiredDir) - static_cast<int>(myTank.getDirection());
    if (angleDiff < 0) angleDiff += NUM_DIRECTIONS ;  // Normalize to positive difference

    // Rotate clockwise (right) 1/8 at a time
//    Direction newDir = static_cast<Direction>((static_cast<int>(myTank.getDirection()) + 1) % 8);
//    return rotateToward(myTank.getDirection(), newDir);

    // Rotate if not facing the desired direction
    if (myTank.getDirection() != desiredDir)
        return rotateToward(myTank.getDirection(), desiredDir);
    bfsPath.erase(bfsPath.begin());
    return Action::MoveFwd;
}


