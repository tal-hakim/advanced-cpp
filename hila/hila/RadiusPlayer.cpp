#include "RadiusPlayer.h"

void::RadiusPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    // create some battle info and pass it to the tank algorithm
    // use satellite view function getObjectAt to get all objects on the board and update battleinfo according to that 
    battle_info.clearInfo();
    std::pair<int, int> called_tank_location = getCalledTankLocation(satellite_view);

    for(int r = called_tank_location.first - radius; r <= called_tank_location.first + radius; ++r) {
        for(int c = called_tank_location.second - radius; c <= called_tank_location.second + radius; ++c) {
            int r_wrap_around = (r + height) % height;
            int c_wrap_around = (c + width) % width;
            char object_sign = satellite_view.getObjectAt(c_wrap_around, r_wrap_around);
            putLocationByChar(object_sign, r_wrap_around, c_wrap_around);
        }
    }

    // Update the tank algorithm with the populated battle info 
    tank.updateBattleInfo(battle_info);
}

std::pair<int, int> RadiusPlayer::getCalledTankLocation(const SatelliteView& satellite_view) const {
    // Iterate over the board to find the called tank location
    for (size_t x = 0; x < height; ++x) {
        for (size_t y = 0; y < width; ++y) {
            if (satellite_view.getObjectAt(y, x) == called_tank_sign) {
                return std::make_pair(x, y);
            }
        }
    }
    return std::make_pair(-1, -1); // default value if not found
}