#pragma once
#include <cmath>
namespace BlasterRules
{
    inline bool CanFire(double Now, double Next, bool Alive, bool Enabled, int Ammo)
    {
        // LAB 1: validate the clock, health, input gate, ammo and cooldown boundary.
        if (!std::isfinite(Now) || !std::isfinite(Next))
        {
            return false;
        }
        if (!Alive || !Enabled || Ammo <= 0)
        {
            return false;
        }
        return Now >= Next;
    }
}