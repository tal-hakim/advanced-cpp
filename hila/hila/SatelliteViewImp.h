#ifndef SATELLITE_VIEW_IMP_H
#define SATELLITE_VIEW_IMP_H    
#include "common/SatelliteView.h"

#include "../configuration/Config.h"
#include <vector>
class SatelliteViewImp : public SatelliteView {
    private:
        static constexpr const char OUT_OF_BOUNDS_SIGN = '&';

        size_t rows;
        size_t columns;
        std::vector<std::vector<char>> satellite_view;
        char out_of_bounds_sign;

    public:
        SatelliteViewImp();
        char getObjectAt(size_t x, size_t y) const override;
        void setSatelliteView(const std::vector<std::vector<char>>& view);
        void setCharAtLocation(const std::pair<int, int>& location, const char& char_to_put);
        void setRowsAndColumns(size_t rows, size_t columns);
};

#endif