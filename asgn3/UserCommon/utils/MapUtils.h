#ifndef ASGN3_MAP_UTILS_H
#define ASGN3_MAP_UTILS_H
#pragma once

#include "Direction.h"
#include "Position.h"
#include "definitions.h"
#include "DirectionUtils.h"
#include <vector>
namespace UserCommon_322213836_212054837 {
    namespace MapUtils {
        // Path finding and distance functions
        std::vector<Position> bfsToEnemy(Position start, Position goal, const std::vector<std::vector<char>> &map);

        int manhattanDistance(Position a, Position b);

        Position wrap(const Position &pos, size_t width, size_t height);

        // Map object checking functions
        bool isWalkable(const Position &pos, const std::vector<std::vector<char>> &map);

        bool isWall(const std::vector<std::vector<char>> &map, const Position &pos);

        bool isMine(const std::vector<std::vector<char>> &map, const Position &pos);

        // Tank-related helper functions
        inline bool isEnemyTank(char obj, int playerId) {
            return obj != '0' + playerId && (obj == PLAYER1_TANK || obj == PLAYER2_TANK);
        }

        inline bool isTank(char obj) {
            return (obj == PLAYER1_TANK || obj == PLAYER2_TANK);
        }

        inline bool isEnemyTankDead(int tankId, const std::vector<std::pair<Position, Direction>> &enemyTanks) {
            if (tankId < 0 || tankId >= static_cast<int>(enemyTanks.size())) {
                return true;
            }
            const Position &pos = enemyTanks[tankId].first;
            return pos.x == -1 || pos.y == -1;  // Check for sentinel value (-1, -1)
        }
    }
}  // namespace UserCommon_322213836_212054837
#endif //ASGN3_MAP_UTILS_H 