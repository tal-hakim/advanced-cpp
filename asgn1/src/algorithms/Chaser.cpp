#include "algorithms/Chaser.h"
#include "objects/Shell.h"
#include "game/Direction.h"
#include "game/Action.h"
#include "game/DirectionUtils.h"
#include <queue>
#include <unordered_set>
#include <string>

namespace {
    Direction directionFromTo(const Position& from, const Position& to) {
        int dx = to.x - from.x;
        int dy = to.y - from.y;
        dx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        dy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

        if (dx == 0 && dy == -1) return Direction::U;
        if (dx == 1 && dy == -1) return Direction::UR;
        if (dx == 1 && dy == 0)  return Direction::R;
        if (dx == 1 && dy == 1)  return Direction::DR;
        if (dx == 0 && dy == 1)  return Direction::D;
        if (dx == -1 && dy == 1) return Direction::DL;
        if (dx == -1 && dy == 0) return Direction::L;
        if (dx == -1 && dy == -1)return Direction::UL;

        return Direction::U; // default fallback
    }

    Action rotateToward(Direction current, Direction target) {
        int rawCurrent = static_cast<int>(current);
        int rawTarget = static_cast<int>(target);
        int diff = (rawTarget - rawCurrent + 8) % 8;

        if (diff == 0) return Action::None;
        if (diff == 1 || diff == 7) return (diff == 1) ? Action::RotateRight_1_8 : Action::RotateLeft_1_8;
        if (diff == 2 || diff == 6) return (diff == 2) ? Action::RotateRight_1_4 : Action::RotateLeft_1_4;
        return (diff <= 4) ? Action::RotateRight_1_8 : Action::RotateLeft_1_8;
    }
}

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
                Shell probe(current, dir, myTank.getPlayerId());
                Position next = board.wrap(probe.getNextPos());

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
                return true;
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
        return rotateToward(myTank.getDirection(), directionFromTo(myTank.getPosition(), opponentTank.getPosition()));

    auto canShootEnemy = [&]() -> bool {
        Position myPos = myTank.getPosition();
        Position enemyPos = opponentTank.getPosition();
        Direction myDir = myTank.getDirection();
    
        Direction toEnemy = directionFromTo(myPos, enemyPos);
        if (toEnemy != myDir) return false;
    
        Position current = board.wrap(myPos + DirectionUtils::dirToVector(myDir));
        while (current != enemyPos) {
            if (board.isWall(current) || board.isMine(current))
                return false;
    
            current = board.wrap(current + DirectionUtils::dirToVector(myDir));
        }
    
        return true;
    };
    
    // 👇 CHECK IF WE SHOULD FIRE
    if (canShootEnemy())
        return Action::Shoot;
    Position next = bfsPath[1];
    Direction desiredDir = directionFromTo(myTank.getPosition(), next);

    if (myTank.getDirection() != desiredDir)
        return rotateToward(myTank.getDirection(), desiredDir);

    return Action::MoveFwd;
}
