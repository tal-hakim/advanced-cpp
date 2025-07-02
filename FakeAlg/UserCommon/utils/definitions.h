//
// Created by talta on 17/04/2025.
//

#ifndef DEFINITIONS_H
#define DEFINITIONS_H
namespace UserCommon_322213836_212054837 {
// Game object constants
    constexpr char WALL = '#';
    constexpr char PLAYER1_TANK = '1';
    constexpr char PLAYER2_TANK = '2';
    constexpr char SHELL = '*';
    constexpr char MINE = '@';
    constexpr char EMPTY = ' ';

// Game constants
    constexpr int BACKWARDS_STEP_COUNT = 2;
    constexpr int UNINITIALIZED = -1;
    constexpr int TIE = 0;
    constexpr int PLAYER_1_ID = 1;
    constexpr int PLAYER_2_ID = 2;
    constexpr int INITIAL_SHELLS_AMOUNT = 16;
    constexpr int SHOOTING_COOLDOWN = 4;
    constexpr int LEFT_ANGLE_1_4 = -2;
    constexpr int RIGHT_ANGLE_1_4 = 2;
    constexpr int LEFT_ANGLE_1_8 = -1;
    constexpr int RIGHT_ANGLE_1_8 = 1;
    constexpr int STALEMATE_STEPS = 40;
    constexpr int NUM_DIRECTIONS = 8;
    constexpr char CURR_TANK = '%';
    constexpr int TIE_SCORE = 1;
    constexpr int WINNING_SCORE = 3;
    constexpr int LOSING_SCORE = 0;
    constexpr int NUM_PLAYERS = 2;
}

#endif //DEFINITIONS_H
