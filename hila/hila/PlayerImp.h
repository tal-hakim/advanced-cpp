#ifndef PLAYERIMP_H
#define PLAYERIMP_H
#include "common/Player.h"
#include "common/SatelliteView.h"
#include "common/TankAlgorithm.h"
#include "SimpleBattleInfo.h"
#include <iostream>
#include <vector>
#include <utility> // for std::pair
#include "../configuration/Config.h"


class PlayerImp : public Player {
     private:
        static constexpr const char OUT_OF_BOUNDS_SIGN = '&';
        static constexpr const char WALL_SIGN = '#';
        static constexpr const char TANK1_SIGN = '1';
        static constexpr const char TANK2_SIGN = '2';
        static constexpr const char CALLED_TANK_SIGN = '%';
        static constexpr const char SHELL_SIGN = '*';
        static constexpr const char MINE_SIGN = '@';
        

    protected:
        SimpleBattleInfo battle_info;
        size_t width;
        size_t height;

        char out_of_bounds_sign;
        char wall_sign;
        char tank1_sign;
        char tank2_sign;
        char called_tank_sign;
        char shell_sign;
        char mine_sign;

        // This function updates the battle_info with the location of the object based on its character representation.    
        void putLocationByChar(char object_sign, int x, int y); 

    public:   
        PlayerImp(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
        virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
        virtual ~PlayerImp() = default;
};

#endif