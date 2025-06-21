#ifndef BFSTANKALGORITHM_H
#define BFSTANKALGORITHM_H
#include "common/TankAlgorithm.h"
#include "common/ActionRequest.h"
#include "../game_objects/CanonDirection.h"
#include "SimpleBattleInfo.h"
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <unordered_set>

class BFSTankAlgorithm : public TankAlgorithm {
    struct State{
        int x, y;
        CanonDirection dir;

        bool operator==(const State& other) const{
            return x == other.x && y == other.y && dir == other.dir;
        }
    };
        
    struct StateHash {
        std::size_t operator()(const State& state) const {
            return std::hash<int>()(state.x)<<1 ^ std::hash<int>()(state.y)<<2 ^ std::hash<int>()(static_cast<int>(state.dir));
        }
    };

    struct QueueNode{
        State state;
        ActionRequest firstAction;
        ActionRequest secondAction;
        int depth;
    };
    
    private:
        int player_id;
        int tank_index;
        int turn_counter;
        int height;
        int width;
        int battle_info_request_period;
        

        // Visited and q are for BFS algorithm, Suggested by ChatGpt
        std::unordered_set<State, StateHash> visited;
        std::queue<QueueNode> q;

        CanonDirection current_canon_direction;
        int last_shoot_turn;

        std::vector<ActionRequest> actions_to_apply;
        std::pair<int, int> getClosestEnemyTank(const std::pair<int, int>& my_location, const std::vector<std::pair<int, int>>& enemy_tanks) const;
        void bfs(const std::pair<int, int>& enemy_location, const SimpleBattleInfo& simple_info);
        
        
        // Helper functions for BFS
        bool canShoot(size_t height, size_t width, const std::pair<int, int>& my_location, const std::pair<int, int>& enemy_location, const CanonDirection& my_direction, const std::vector<std::pair<int, int>>& wall_locations) const;
        bool clearPathFromSrcToDst(size_t height, size_t width, const int src_x, const int src_y, const int dst_x, const int dst_y, const CanonDirection dir, const std::vector<std::pair<int, int>>&  bad_moves_locations) const;
        bool canMove(int new_x, int new_y, const std::vector<std::pair<int, int>>& wall_locations, const std::vector<std::pair<int, int>>& mine_locations, const std::vector<std::pair<int, int>>& tanks1_locations ,const std::vector<std::pair<int, int>>& tanks2_locations) const;
        std::pair<int,int> getNextForwardLocation(const std::pair<int, int>& current_location, const CanonDirection& dir) const;
        CanonDirection rotate(CanonDirection cur_dir, int rotation);
        bool tryShoot(const QueueNode& current, const SimpleBattleInfo& info, const std::pair<int, int>& enemy_location);
        void tryMoveForward(const QueueNode& current, const SimpleBattleInfo& info);
        void tryRotations(const QueueNode& current);

    public:
        BFSTankAlgorithm(int player_id, int tank_index,int battle_info_request_period);
        virtual ActionRequest getAction() override;
        virtual void updateBattleInfo(BattleInfo& info) override; 
        virtual ~BFSTankAlgorithm() override = default;
};

#endif