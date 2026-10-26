#include "Week04/TargetCrate.h"
#include "Week04/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
ATargetCrate::ATargetCrate()
{
    PrimaryActorTick.bCanEverTick=false;
    CrateMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh")); SetRootComponent(CrateMesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    CrateMesh->SetStaticMesh(Cube.Object); CrateMesh->SetRelativeScale3D(FVector(.32f,.24f,.32f));
    CrateMesh->SetCollisionProfileName(TEXT("BlockAll"));
    HealthComponent=CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}
void ATargetCrate::BeginPlay()
{
    Super::BeginPlay();
    HealthComponent->OnHealthChanged.AddUniqueDynamic(this,&ATargetCrate::HealthChanged);
    HealthComponent->OnDied.AddUniqueDynamic(this,&ATargetCrate::HandleDeath);
}
void ATargetCrate::EndPlay(const EEndPlayReason::Type Reason)
{
    HealthComponent->OnHealthChanged.RemoveDynamic(this,&ATargetCrate::HealthChanged);
    HealthComponent->OnDied.RemoveDynamic(this,&ATargetCrate::HandleDeath);
    Super::EndPlay(Reason);
}
void ATargetCrate::HealthChanged(float Health,float Maximum) { ++ChangeEvents; }
void ATargetCrate::HandleDeath()
{
    ++DeathEvents;
    // Keep the actor alive for the already-dead acceptance test. Blueprint owns its visible response.
    ReceiveDeathVisual();
}
