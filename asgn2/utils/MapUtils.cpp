#include <queue>
#include <unordered_set>
#include "utils/MapUtils.h"
#include "definitions.h"
#include <limits>

namespace MapUtils {
    std::vector<Position> bfsToEnemy(Position start, Position goal, const std::vector<std::vector<char>>& map)  {
        const int maxDepth = 15;
        std::queue<std::pair<Position, std::vector<Position>>> q;
        std::unordered_set<std::string> visited;
        std::vector<Position> bestPath;
        int bestDistance = std::numeric_limits<int>::max();
        size_t mapWidth = map.size();
        size_t mapHeight = map[0].size();

        q.push({start, {start}});
        visited.insert(start.toString());

        while (!q.empty()) {
            auto [current, path] = q.front(); q.pop();

            // Record best path so far (closest to goal)
            int dist = manhattanDistance(current, goal);
            if (dist < bestDistance) {
                bestDistance = dist;
                bestPath = path;
            }

            if (current == goal) {
                return path;
            }

            if (path.size() > maxDepth)
                continue;

            for (int d = 0; d < NUM_DIRECTIONS; ++d) {
                Direction dir = static_cast<Direction>(d);
                Position next = wrap(current + DirectionUtils::dirToVector(dir), mapWidth, mapHeight);

                // Skip if not walkable or already visited
                if (!isWalkable(next, map) || visited.count(next.toString()))
                    continue;

                // If this is not the goal position and we're close to current position, check for tanks
                if (next != goal && manhattanDistance(current, next) <= 2) {
                    char obj = map[next.x][next.y];
                    if (isTank(obj)) {
                        continue;  // Skip positions with tanks if we're close to current position
                    }
                }

                visited.insert(next.toString());
                auto newPath = path;
                newPath.push_back(next);
                q.push({next, newPath});
            }
        }

        // Fallback to best-effort path (even if goal not reached)
        return bestPath;
    }

    int manhattanDistance(Position a, Position b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    Position wrap(const Position& pos, size_t width, size_t height) {
        int x = (pos.x + width) % width;
        int y = (pos.y + height) % height;
        return Position{x, y};
    }

    bool isWalkable(const Position& pos, const std::vector<std::vector<char>>& map){
        return !isWall(map, pos) &&
               !isMine(map, pos);
    }

    // Helper functions to work directly with map data
    bool isWall(const std::vector<std::vector<char>>& map, const Position& pos) {
        return map[pos.x][pos.y] == WALL;
    }

    bool isMine(const std::vector<std::vector<char>>& map, const Position& pos) {
        return map[pos.x][pos.y] == MINE;
    }
}



