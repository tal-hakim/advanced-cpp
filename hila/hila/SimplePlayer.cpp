#include "SimplePlayer.h"

void::SimplePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    battle_info.clearInfo();

    // Iterate over the board using satellite_view and populate battle_info
    for (size_t x = 0; x < height; ++x) {
        for (size_t y = 0; y < width; ++y) {
            char object_sign = satellite_view.getObjectAt(y, x);
            putLocationByChar(object_sign, x, y);
        }
    }
    
    // Update the tank algorithm with the populated battle info 
    tank.updateBattleInfo(battle_info);
}