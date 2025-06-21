#include "SatelliteViewImp.h"

SatelliteViewImp::SatelliteViewImp()
    : out_of_bounds_sign(Config::getInstance().get("out_of_bounds_sign", std::string(1, OUT_OF_BOUNDS_SIGN))[0]){}

char SatelliteViewImp::getObjectAt(size_t x, size_t y) const {
    if (x >= rows || y >= columns) {
        return out_of_bounds_sign;
    }
    return satellite_view[x][y];
      
}

void SatelliteViewImp::setSatelliteView(const std::vector<std::vector<char>>& view) {
    satellite_view = view;
}

void SatelliteViewImp::setCharAtLocation(const std::pair<int, int>& location, const char& char_to_put) {
    satellite_view[location.first][location.second] = char_to_put;
}

void SatelliteViewImp::setRowsAndColumns(size_t rows, size_t columns) {
    this->rows = rows;
    this->columns = columns;
}