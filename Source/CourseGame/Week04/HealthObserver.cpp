#include "Week04/HealthObserver.h"
#include "Week04/HealthComponent.h"
AHealthObserver::AHealthObserver() { PrimaryActorTick.bCanEverTick=false; }
void AHealthObserver::AttachToHealth(UHealthComponent* NewSource)
{
    Detach();
    if (!IsValid(NewSource)) return;
    Source=NewSource;
    NewSource->OnHealthChanged.AddUniqueDynamic(this,&AHealthObserver::Changed);
    NewSource->OnDied.AddUniqueDynamic(this,&AHealthObserver::Died);
    // Initial snapshot is not a health-change event.
    LastHealth=NewSource->GetHealth(); ++Snapshots;
}
void AHealthObserver::Detach()
{
    if (Source.IsValid())
    {
        Source->OnHealthChanged.RemoveDynamic(this,&AHealthObserver::Changed);
        Source->OnDied.RemoveDynamic(this,&AHealthObserver::Died);
    }
    Source.Reset();
}
void AHealthObserver::EndPlay(const EEndPlayReason::Type Reason) { Detach(); Super::EndPlay(Reason); }
void AHealthObserver::Changed(float Health,float Maximum)
{
    LastHealth=Health; ++ChangeEvents;
    UE_LOG(LogTemp,Display,TEXT("W4_OBSERVER %s health=%.1f/%.1f changes=%d"),*GetName(),Health,Maximum,ChangeEvents);
    if (bReenterOnChange && Source.IsValid()) Source->ApplyDamage(1.f);
}
void AHealthObserver::Died() { ++DeathEvents; }
