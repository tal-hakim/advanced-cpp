#ifndef RADIUS_PLAYER_H
#define RADIUS_PLAYER_H
#include "PlayerImp.h"


class RadiusPlayer : public PlayerImp {
    private:
        int radius;
        std::pair<int, int> getCalledTankLocation(const SatelliteView& satellite_view) const;
        
    public:
        RadiusPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells, int radius)
            : PlayerImp(player_index, x, y, max_steps, num_shells),
              radius(radius) {}

        virtual ~RadiusPlayer() = default;
        virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};

#endif