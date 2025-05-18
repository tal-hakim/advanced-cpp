//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_SATELLITEVIEW_H
#define ASGN2_SATELLITEVIEW_H

#include <cstddef>

/**
 * Interface for viewing the game board from a tank's perspective.
 * The getObjectAt method returns characters representing different objects:
 * ' ' - Empty space
 * '#' - Wall
 * '@' - Mine
 * '1' - Player 1's tank
 * '2' - Player 2's tank
 * '%' - Current tank's position
 * '&' - Outside the battlefield
 */
class SatelliteView {
public:
    virtual ~SatelliteView() {}
    
    /**
     * Get the object at the specified position
     * @param x The x coordinate
     * @param y The y coordinate
     * @return A character representing the object at that position
     */
    virtual char getObjectAt(size_t x, size_t y) const = 0;
};

#endif //ASGN2_SATELLITEVIEW_H
