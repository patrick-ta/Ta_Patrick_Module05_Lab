#pragma once
#include <cmath>
#include <algorithm>
namespace Week04Rules
{
struct Change { float Value; bool Changed; bool Died; };
inline float ValidMaximum(float Value) { return std::isfinite(Value) && Value>0.f ? Value : 100.f; }
inline Change Damage(float Health,float Maximum,float Amount)
{
    if (!std::isfinite(Amount) || Amount<=0.f || Health<=0.f) return {Health,false,false};
    const float Next=std::clamp(Health-Amount,0.f,Maximum);
    return {Next,Next!=Health,Health>0.f && Next==0.f};
}
inline Change Heal(float Health,float Maximum,float Amount)
{
    if (!std::isfinite(Amount) || Amount<=0.f || Health<=0.f) return {Health,false,false};
    const float Next=Health+std::min(Amount,Maximum-Health);
    return {Next,Next!=Health,false};
}
}
