#include "BFSTankAlgorithm.h"


BFSTankAlgorithm::BFSTankAlgorithm(int player_id, int tank_index, int battle_info_request_period): player_id(player_id), tank_index(tank_index), battle_info_request_period(battle_info_request_period), last_shoot_turn(-1) {
    current_canon_direction = (player_id == 1) ? CanonDirection::LEFT : CanonDirection::RIGHT;
    turn_counter = 0;
    height = 0;
    width = 0;
}

ActionRequest BFSTankAlgorithm::getAction() {
    turn_counter++;
    
    // We decided to request battle info every 3 turns, so if the turn counter is 1, we request battle info
    if(turn_counter % battle_info_request_period == 1){
        return ActionRequest::GetBattleInfo;
    }

    ActionRequest cur_action = actions_to_apply[0];
    // Rotate the canon direction based on the action
    switch (cur_action) 
    {
        case ActionRequest::RotateLeft45:
            current_canon_direction = rotate(current_canon_direction, -1);
            break;
        
        case ActionRequest::RotateRight45:
            current_canon_direction = rotate(current_canon_direction, 1);
            break;

        case ActionRequest::RotateLeft90:   
            current_canon_direction = rotate(current_canon_direction, -2);
            break;

        case ActionRequest::RotateRight90:      
            current_canon_direction = rotate(current_canon_direction, 2);
            break;

        case ActionRequest::Shoot:
            last_shoot_turn = turn_counter;
            break;

        default:
            break;    
    }
  
    actions_to_apply.erase(actions_to_apply.begin());
    return cur_action;
} 

/* We decided that updateBattleInfo is a complex function that calculates the next 2 actions.
*  GetAction will return the result of the calculation.
*/
void BFSTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    actions_to_apply.clear();
    SimpleBattleInfo& simple_info = dynamic_cast<SimpleBattleInfo&>(info); // Downcast is allowed

    if(height == 0 && width == 0){
        height = simple_info.getHeight();
        width = simple_info.getWidth();
    }

    // Find closest enemy tank
    std::pair<int, int> my_location = simple_info.getCalledTankLocation();
    std::vector<std::pair<int, int>> enemy_tanks = (player_id==1) ? simple_info.getTanks2Locations() : simple_info.getTanks1Locations();
    std::pair<int, int> closest_enemy = getClosestEnemyTank(my_location, enemy_tanks);

    // If closest enemy is not found, try to move using the function can_move and if not possible rotate
    if(closest_enemy.first == -1 && closest_enemy.second == -1){
        // Check if can move forward
        std::pair<int, int> next_location = getNextForwardLocation(my_location, current_canon_direction);
        if(canMove(next_location.first, next_location.second, simple_info.getWallsLocations(), simple_info.getMinesLocations(), simple_info.getTanks1Locations(), simple_info.getTanks2Locations())){
            actions_to_apply.push_back(ActionRequest::MoveForward);
            actions_to_apply.push_back(ActionRequest::GetBattleInfo);
        }
        else{
            actions_to_apply.push_back(ActionRequest::RotateRight45);
            actions_to_apply.push_back(ActionRequest::GetBattleInfo);
        }
    }
    else{
        /* Calculate next 2 action to get to this enemy, avoid walls and mines and shooting our tanks
        *  save those 2 steps in the actions vector */
        bfs(closest_enemy, simple_info);
    }
   
}

bool BFSTankAlgorithm::tryShoot(const QueueNode& current, const SimpleBattleInfo& info, const std::pair<int, int>& enemy_location) {
    int x = current.state.x;
    int y = current.state.y;
    CanonDirection dir = current.state.dir;

    if (canShoot(info.getHeight(), info.getWidth(), {x, y}, enemy_location, dir, info.getWallsLocations())) {
        ActionRequest first = current.firstAction;
        ActionRequest second = current.secondAction;

        if (current.depth == 0 && (last_shoot_turn == -1 || turn_counter - last_shoot_turn >= 4)) {
            first = ActionRequest::Shoot;
        }
        else if (current.depth == 1 && (last_shoot_turn == -1 || turn_counter - last_shoot_turn >= 3)) {
            second = ActionRequest::Shoot;
        }

        actions_to_apply.push_back(first);
        actions_to_apply.push_back(second);
        
        return true;
    }
    return false;
}

void BFSTankAlgorithm::tryMoveForward(const QueueNode& current, const SimpleBattleInfo& info) {
    auto [x, y] = getNextForwardLocation({current.state.x, current.state.y}, current.state.dir);

    if (canMove(x, y, info.getWallsLocations(), info.getMinesLocations(), info.getTanks1Locations(), info.getTanks2Locations())) {
        State new_state = {x, y, current.state.dir};
        if (visited.find(new_state) == visited.end()) {
            visited.insert(new_state);
            ActionRequest act1 = current.depth == 0 ? ActionRequest::MoveForward : current.firstAction;
            ActionRequest act2 = current.depth == 1 ? ActionRequest::MoveForward : current.secondAction;
            q.push(QueueNode{new_state, act1, act2, current.depth + 1});
        }
    }
}

void BFSTankAlgorithm::tryRotations(const QueueNode& current) {
    static const std::vector<std::pair<ActionRequest, CanonDirection>> rotations = {
        {ActionRequest::RotateLeft45,  CanonDirection((static_cast<int>(current.state.dir) + 8 - 1) % 8)},
        {ActionRequest::RotateLeft90,  CanonDirection((static_cast<int>(current.state.dir) + 8 - 2) % 8)},
        {ActionRequest::RotateRight45, CanonDirection((static_cast<int>(current.state.dir) + 1) % 8)},
        {ActionRequest::RotateRight90, CanonDirection((static_cast<int>(current.state.dir) + 2) % 8)},
    };

    for (const auto& [rotation_act, new_dir] : rotations) {
        State new_state = {current.state.x, current.state.y, new_dir};
        if (visited.find(new_state) == visited.end()) {
            visited.insert(new_state);
            ActionRequest act1 = current.depth == 0 ? rotation_act : current.firstAction;
            ActionRequest act2 = current.depth == 1 ? rotation_act : current.secondAction;
            q.push(QueueNode{new_state, act1, act2, current.depth + 1});
        }
    }
}

void BFSTankAlgorithm::bfs(const std::pair<int, int>& enemy_location, const SimpleBattleInfo& simple_info ){
    visited.clear();
    q = std::queue<QueueNode>();
    actions_to_apply.clear();

    CanonDirection dir = current_canon_direction;
    State start_state = {simple_info.getCalledTankLocation().first, simple_info.getCalledTankLocation().second, dir};
    q.push(QueueNode{start_state, ActionRequest::DoNothing, ActionRequest::DoNothing, 0});
    visited.insert(start_state);

    while(!q.empty()){
        QueueNode current = q.front();
        q.pop();
        if(current.depth > 10) break;

        // Check for shooting opportunity
        if (tryShoot(current, simple_info, enemy_location)) return;

        // Move forward
        tryMoveForward(current, simple_info);

        // Rotation options
        tryRotations(current);
    }

    // fallback if nothing was found
    actions_to_apply.push_back(ActionRequest::RotateLeft45);
    actions_to_apply.push_back(ActionRequest::RotateLeft45);
}

bool BFSTankAlgorithm::canShoot(size_t height, size_t width, const std::pair<int, int>& my_location, const std::pair<int, int>& enemy_location, const CanonDirection& my_direction, const std::vector<std::pair<int, int>>& wall_locations) const {
    return clearPathFromSrcToDst(height, width, my_location.first, my_location.second, enemy_location.first, enemy_location.second, my_direction, wall_locations);
}

bool BFSTankAlgorithm::canMove(int new_x, int new_y,
    const std::vector<std::pair<int, int>>& wall_locations, const std::vector<std::pair<int, int>>& mine_locations, 
    const std::vector<std::pair<int, int>>& tanks1_locations ,const std::vector<std::pair<int, int>>& tanks2_locations) const
    {
        // Used chatGpt to learn how to combine vectors, prompt was "How to combine 4 vectors in c++ in the most efficient way"
        std::vector<std::pair<int, int>> bad_moves_locations;
        bad_moves_locations.reserve(wall_locations.size() + mine_locations.size() + tanks1_locations.size() + tanks2_locations.size());
        bad_moves_locations.insert(bad_moves_locations.end(), wall_locations.begin(), wall_locations.end());
        bad_moves_locations.insert(bad_moves_locations.end(), mine_locations.begin(), mine_locations.end());    
        bad_moves_locations.insert(bad_moves_locations.end(), tanks1_locations.begin(), tanks1_locations.end());
        bad_moves_locations.insert(bad_moves_locations.end(), tanks2_locations.begin(), tanks2_locations.end());
        for(const auto& bad : bad_moves_locations) {
            if (new_x == bad.first && new_y == bad.second) {
                return false;
            }
        }
        return true;
    }

bool BFSTankAlgorithm::clearPathFromSrcToDst(size_t height, size_t width, const int src_x, const int src_y, const int dst_x, const int dst_y, const CanonDirection dir, const std::vector<std::pair<int, int>>& bad_moves_locations) const{
    int cx = src_x;
    int cy = src_y;
    int ox = dst_x;
    int oy = dst_y;
 
    
    for (size_t steps = 0; steps < std::max(height, width); ++steps) {
        if (cx == ox && cy == oy) {
            return true;
        }
        for (const auto& bad : bad_moves_locations) {
            if (cx == bad.first && cy == bad.second) {
                return false;
            }
        }

        switch (dir) {
            case CanonDirection::UP:
                cx = (cx - 1 + height) % height;
                break;
            case CanonDirection::DOWN:
                cx = (cx + 1) % height;
                break;
            case CanonDirection::LEFT:
                cy = (cy - 1 + width) % width;
                break;
            case CanonDirection::RIGHT:
                cy = (cy + 1) % width;
                break;
            case CanonDirection::UP_RIGHT:
                cx = (cx - 1 + height) % height;
                cy = (cy + 1) % width;
                break;
            case CanonDirection::UP_LEFT:
                cx = (cx - 1 + height) % height;
                cy = (cy - 1 + width) % width;
                break;
            case CanonDirection::DOWN_RIGHT:
                cx = (cx + 1) % height;
                cy = (cy + 1) % width;
                break;
            case CanonDirection::DOWN_LEFT:
                cx = (cx + 1) % height;
                cy = (cy - 1 + width) % width;
                break;
        }
    }
    return false; 
}

CanonDirection BFSTankAlgorithm::rotate(CanonDirection cur_dir, int rotation) {
    // Used ChatGpt to cast the direction to number and preforme rotation as addition/substraction
    int new_dir = static_cast<int>(cur_dir) + rotation;
    if (new_dir < 0) {
        new_dir += 8; // Wrap around to the last direction
    } else if (new_dir >= 8) {
        new_dir -= 8; // Wrap around to the first direction
    }
    return static_cast<CanonDirection>(new_dir);
}

std::pair<int, int> BFSTankAlgorithm::getClosestEnemyTank(const std::pair<int, int>& my_location, const std::vector<std::pair<int, int>>& enemy_tanks) const{
    // There shouldn't be a case were the return value is {-1,-1} because if the enemy doesn't have any live tanks the game will finish.
    int min_dist = std::numeric_limits<int>::max();
    std::pair<int, int> closest_enemy = {-1, -1};
    for (const auto& enemy_loc : enemy_tanks) {
        int dist = abs(my_location.first - enemy_loc.first) + abs(my_location.second - enemy_loc.second);
        if (dist < min_dist) {
            min_dist = dist;
            closest_enemy = enemy_loc;
        }
    }
    return closest_enemy;
}

std::pair<int, int> BFSTankAlgorithm::getNextForwardLocation(const std::pair<int, int>& current_location, const CanonDirection& dir) const {
    int new_x = current_location.first;
    int new_y = current_location.second;

    switch(dir){
        case CanonDirection::UP: new_x--; break;
        case CanonDirection::DOWN: new_x++; break;
        case CanonDirection::LEFT: new_y--; break;
        case CanonDirection::RIGHT: new_y++; break;
        case CanonDirection::UP_RIGHT: new_x--; new_y++; break;
        case CanonDirection::UP_LEFT: new_x--; new_y--; break;
        case CanonDirection::DOWN_LEFT: new_x++; new_y--; break;
        case CanonDirection::DOWN_RIGHT: new_x++; new_y++; break;
        default: break;
    }
    new_x = (new_x + height) % height;
    new_y = (new_y + width) % width;
    return std::make_pair(new_x, new_y);
}