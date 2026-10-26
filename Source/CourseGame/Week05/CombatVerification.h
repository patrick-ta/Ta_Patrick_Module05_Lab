#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Week05/CourseProjectile.h"
#include "CombatVerification.generated.h"
class ATargetCrate;class UBlasterComponent;class ARunnerCharacter;
UCLASS() class COURSEGAME_API AFailedSpawnProjectile:public ACourseProjectile
{GENERATED_BODY() public:virtual void BeginPlay() override {Super::BeginPlay();Destroy();}};
UCLASS() class COURSEGAME_API ACombatVerification:public AActor
{
 GENERATED_BODY()
public:
 ACombatVerification();virtual void Tick(float Dt) override;
 UFUNCTION() void Reenter(float Health,float Maximum);
private:
 void Check(const TCHAR* Name,bool Passed);void StartTests();void FinishTests();
 UPROPERTY() TObjectPtr<ACourseProjectile> ReentrantProjectile;
 UPROPERTY() TArray<TObjectPtr<AActor>> Fixtures;
 UPROPERTY() TArray<TObjectPtr<ATargetCrate>> SweepTargets;
 UPROPERTY() TArray<TObjectPtr<ACourseProjectile>> SweepShots;
 UPROPERTY() TObjectPtr<ACourseProjectile> Miss;
 UPROPERTY() TObjectPtr<UBlasterComponent> EndedWeapon;
 UPROPERTY() TObjectPtr<ARunnerCharacter> Shooter;
 float Elapsed=0;int32 Phase=0;bool Failed=false;
 FString Results=TEXT("check,result\n");bool Baseline=false;
};
