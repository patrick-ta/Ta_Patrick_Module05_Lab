#include "Week04/InteractionTargets.h"
#include "Week04/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
ACourseDoor::ACourseDoor()
{
    Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));SetRootComponent(Mesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));Mesh->SetStaticMesh(Cube.Object);
    Mesh->SetRelativeScale3D(FVector(.15f,.20f,.7f));Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}
bool ACourseDoor::Interact_Implementation(AActor* Interactor)
{
    if(!HasAuthority()||!IsValid(Interactor))return false;
    ++Requests;bOpen=!bOpen;Mesh->SetRelativeRotation(FRotator(0,bOpen?90.f:0.f,0));
    Mesh->SetCollisionResponseToChannel(ECC_Pawn,bOpen?ECR_Ignore:ECR_Block);return true;
}
AInteractionPickup::AInteractionPickup()
{
    Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));SetRootComponent(Mesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));Mesh->SetStaticMesh(Sphere.Object);
    Mesh->SetRelativeScale3D(FVector(.18f));Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}
bool AInteractionPickup::Interact_Implementation(AActor* Interactor)
{
    if(!HasAuthority()||!IsValid(Interactor)||bCollected)return false;
    bCollected=true;SetActorHiddenInGame(true);SetActorEnableCollision(false);return true;
}
ARechargeStation::ARechargeStation()
{
    Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StationMesh"));SetRootComponent(Mesh);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));Mesh->SetStaticMesh(Cylinder.Object);
    Mesh->SetRelativeScale3D(FVector(.20f,.20f,.35f));Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}
bool ARechargeStation::Interact_Implementation(AActor* Interactor)
{
    if(!HasAuthority()||!IsValid(Interactor))return false;
    auto* Health=Interactor->FindComponentByClass<UHealthComponent>();if(!Health)return false;
    const float Before=Health->GetHealth();Health->Heal(25.f);return Health->GetHealth()!=Before;
}
