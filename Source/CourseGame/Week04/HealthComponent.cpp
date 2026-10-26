#include "Week04/HealthComponent.h"
#include "Week04/HealthRules.h"
#include "GameFramework/Actor.h"
UHealthComponent::UHealthComponent() { PrimaryComponentTick.bCanEverTick=false; }
void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    MaxHealth=Week04Rules::ValidMaximum(MaxHealth); Health=MaxHealth;
}
void UHealthComponent::ApplyDamage(float Amount)
{
    // Only the gameplay authority may commit state. This module supplies no network transport.
    if (!GetOwner() || !GetOwner()->HasAuthority() || bPublishing) return;
    const auto Change=Week04Rules::Damage(Health,MaxHealth,Amount);
    if (!Change.Changed) return;
    Health=Change.Value;
    // Commit first, then notify. Nested mutations from listeners are rejected.
    TGuardValue<bool> Guard(bPublishing,true);
    OnHealthChanged.Broadcast(Health,MaxHealth);
    if (Change.Died) OnDied.Broadcast();
}
void UHealthComponent::Heal(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || bPublishing) return;
    const auto Change=Week04Rules::Heal(Health,MaxHealth,Amount);
    if (!Change.Changed) return;
    Health=Change.Value;
    TGuardValue<bool> Guard(bPublishing,true);
    OnHealthChanged.Broadcast(Health,MaxHealth);
}
