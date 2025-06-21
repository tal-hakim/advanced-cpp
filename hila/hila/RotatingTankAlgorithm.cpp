#include "RotatingTankAlgorithm.h"
#include "SimpleBattleInfo.h"
#include <iostream>

RotatingTankAlgorithm::RotatingTankAlgorithm(int player_id, int tank_index): player_id(player_id), tank_index(tank_index), turn_counter(0)  {}
ActionRequest RotatingTankAlgorithm::getAction() {
    return ActionRequest::RotateLeft45;
}   
void RotatingTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    SimpleBattleInfo& simple_info = dynamic_cast<SimpleBattleInfo&>(info);
    std::cout << simple_info.getCalledTankLocation().first << " " << simple_info.getCalledTankLocation().second << std::endl;
}

int RotatingTankAlgorithm::getPlayerId() {
    return player_id;
} 

int RotatingTankAlgorithm::getTankIndex(){
    return tank_index;
}
