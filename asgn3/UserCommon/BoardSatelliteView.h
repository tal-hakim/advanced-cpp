//
// Created by talta on 10/05/2025.
//

#ifndef ASGN2_BOARDSATELLITEVIEW_H
#define ASGN2_BOARDSATELLITEVIEW_H
#include "../Common/SatelliteView.h"
#include <vector>


namespace UserCommon_322213836_212054837 {
    class BoardSatelliteView : public SatelliteView {
    private:
        std::vector<std::vector<char>> boardView;  // boardView[y][x] for row-major order

    public:
        explicit BoardSatelliteView(std::vector<std::vector<char>> matrix)
                : boardView(std::move(matrix)) {}

        void setObjectAt(size_t x, size_t y, char value) {
            if (x < boardView.size() && y < boardView[x].size()) {
                boardView[x][y] = value;
            }
        }

        char getObjectAt(size_t x, size_t y) const override {
            if (x >= boardView.size() || y >= boardView[x].size()) {
                return '&';  // Outside the battlefield
            }
            return boardView[x][y];
        }

        size_t getWidth() const {
            return boardView.empty() ? 0 : boardView[0].size();
        }

        size_t getHeight() const {
            return boardView.size();
        }
    };
}
#endif //ASGN2_BOARDSATELLITEVIEW_H
