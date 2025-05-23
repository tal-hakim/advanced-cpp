//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_BOARDSATELLITEVIEW_H
#define ASGN2_BOARDSATELLITEVIEW_H
#include "SatelliteView.h"
#include <vector>

class BoardSatelliteView : public SatelliteView {
private:
    std::vector<std::vector<char>> boardView;  // boardView[y][x] for row-major order

public:
    explicit BoardSatelliteView(std::vector<std::vector<char>> matrix)
            : boardView(std::move(matrix)) {}

    void setObjectAt(size_t x, size_t y, char value) {
        if (y < boardView.size() && x < boardView[y].size()) {
            boardView[y][x] = value;
        }
    }

    char getObjectAt(size_t x, size_t y) const override {
        if (y >= boardView.size() || x >= boardView[y].size()) {
            return '&';  // Outside the battlefield
        }
        return boardView[y][x];
    }

    size_t getWidth() const {
        return boardView.empty() ? 0 : boardView[0].size();
    }

    size_t getHeight() const {
        return boardView.size();
    }
};

#endif //ASGN2_BOARDSATELLITEVIEW_H
