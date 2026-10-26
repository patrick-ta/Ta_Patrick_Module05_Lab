#include "Week05/CombatController.h"
#include "Week05/BlasterComponent.h"
#include "Week05/CourseProjectile.h"
#include "Week04/TargetCrate.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/InputComponent.h"
void ACombatController::SetupInputComponent()
{
 Super::SetupInputComponent();
 InputComponent->BindKey(EKeys::J,IE_Pressed,this,&ACombatController::Fire);
 InputComponent->BindKey(EKeys::T,IE_Pressed,this,&ACombatController::ResetRange);
 InputComponent->BindKey(EKeys::V,IE_Pressed,this,&ACombatController::ToggleSpeed);
}
void ACombatController::OnPossess(APawn* InPawn)
{
 Super::OnPossess(InPawn);
 UClass* WeaponClass=LoadClass<UBlasterComponent>(nullptr,TEXT("/Game/Course/Week05/BP_Weapon.BP_Weapon_C"));
 Weapon=NewObject<UBlasterComponent>(InPawn,WeaponClass?WeaponClass:UBlasterComponent::StaticClass());
 InPawn->AddInstanceComponent(Weapon);Weapon->RegisterComponent();
 for(TActorIterator<ATargetCrate> It(GetWorld());It;++It){Target=*It;break;}
}
void ACombatController::Fire(){if(Weapon)Weapon->TryFire();}
void ACombatController::ToggleSpeed(){if(Weapon)Weapon->ProjectileSpeed=Weapon->ProjectileSpeed<1000?1800:600;}
void ACombatController::ResetRange()
{
 for(TActorIterator<ACourseProjectile> It(GetWorld());It;++It)It->Destroy();
 for(TActorIterator<ATargetCrate> It(GetWorld());It;++It)It->Destroy();
 UClass* Class=LoadClass<ATargetCrate>(nullptr,TEXT("/Game/Course/Week04/BP_TargetCrate.BP_TargetCrate_C"));
 FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 Target=GetWorld()->SpawnActor<ATargetCrate>(Class,FVector(380,0,34),FRotator::ZeroRotator,Params);
 RestartRunner();
}
