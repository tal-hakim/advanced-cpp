//
// Created by talta on 21/06/2025.
//

#ifndef ASGN3_SATELLITEVIEW_H
#define ASGN3_SATELLITEVIEW_H
//-------------------------------
// common/SatelliteView.h
//-------------------------------

class SatelliteView {
public:
    virtual ~SatelliteView() {}
    virtual char getObjectAt(size_t x, size_t y) const = 0;
};
#endif //ASGN3_SATELLITEVIEW_H
