//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_SATELLITEVIEW_H
#define ASGN2_SATELLITEVIEW_H

#include <cstddef>

class SatelliteView {
public:
    virtual ~SatelliteView() {}
    virtual char getObjectAt(size_t x, size_t y) const = 0;
};

#endif //ASGN2_SATELLITEVIEW_H
