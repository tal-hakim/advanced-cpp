//
// Created by talta on 21/06/2025.
//

#include <vector>

#ifndef ASGN3_GAMERESULT_H
#define ASGN3_GAMERESULT_H
struct GameResult {
    int winner; // 0 = tie
    enum Reason { ALL_TANKS_DEAD, MAX_STEPS, ZERO_SHELLS };
    Reason reason;
    std::vector<std::size_t> remaining_tanks; // index 0 = player 1, etc.
};

#endif //ASGN3_GAMERESULT_H
