#include "SimpleBattleInfo.h"
#include <utility> // for std::pair
#include <vector>

size_t SimpleBattleInfo::getHeight() const {
    return height;
}
size_t SimpleBattleInfo::getWidth() const {
    return width;
}
const std::vector<std::pair<int, int>>& SimpleBattleInfo::getTanks1Locations() const {
    return tanks1_locations;
}
const std::vector<std::pair<int, int>>& SimpleBattleInfo::getTanks2Locations() const {
    return tanks2_locations;
}
const std::pair<int, int>& SimpleBattleInfo::getCalledTankLocation() const {
    return called_tank_location;
}
const std::vector<std::pair<int, int>>& SimpleBattleInfo::getShellsLocations() const {
    return shells_locations;
}
const std::vector<std::pair<int, int>>& SimpleBattleInfo::getMinesLocations() const {
    return mines_locations;
}
const std::vector<std::pair<int, int>>& SimpleBattleInfo::getWallsLocations() const {
    return walls_locations;
}

void SimpleBattleInfo::addTanks1Location(std::pair<int, int> tank1) {
    tanks1_locations.push_back(std::move(tank1));
}
void SimpleBattleInfo::addTanks2Location(std::pair<int, int> tank2) {
    tanks2_locations.push_back(std::move(tank2));
}

void SimpleBattleInfo::addCalledTankLocation(const std::pair<int, int> called_tank_location) {
    this->called_tank_location = called_tank_location;
}

void SimpleBattleInfo::addShellLocation(std::pair<int, int> shell) {
    shells_locations.push_back(std::move(shell));
}

void SimpleBattleInfo::addMineLocation(std::pair<int, int> mine) {
    mines_locations.push_back(std::move(mine));
}
void SimpleBattleInfo::addWallLocation(std::pair<int, int> wall) {
    walls_locations.push_back(std::move(wall));
}

void SimpleBattleInfo::clearInfo() {
    tanks1_locations.clear();
    tanks2_locations.clear();
    called_tank_location = std::make_pair(-1, -1); // default value
    shells_locations.clear();
    mines_locations.clear();
    walls_locations.clear();
}