#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterComponent.generated.h"
class ACourseProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlasterShotAccepted);
UCLASS(Blueprintable,ClassGroup=(Course),meta=(BlueprintSpawnableComponent))
class COURSEGAME_API UBlasterComponent:public UActorComponent
{
 GENERATED_BODY()
public:
 UBlasterComponent();
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Blaster") TSubclassOf<ACourseProjectile> ProjectileClass;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Blaster") float Cooldown=.25f;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Blaster") float Damage=25.f;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Blaster") float ProjectileSpeed=600.f;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Blaster") int32 Ammo=20;
 UPROPERTY(BlueprintReadOnly,Category="Blaster") int32 Accepted=0;
 UPROPERTY(BlueprintReadOnly,Category="Blaster") int32 Rejected=0;
 UPROPERTY(BlueprintReadOnly,Category="Blaster") bool bReady=true;
 UPROPERTY(BlueprintAssignable,Category="Blaster") FBlasterShotAccepted OnShotAccepted;
 FString LastReason=TEXT("Ready");
 double NextAllowedFireTime=0.;
 TWeakObjectPtr<ACourseProjectile> LastProjectile;
 UFUNCTION(BlueprintCallable,Category="Blaster") bool TryFire();
 bool CanFire(double Now) const;
 bool HasReadyTimer() const;
 FTimerHandle GetReadyTimerHandle() const {return ReadyTimer;}
private:
 FTimerHandle ReadyTimer;bool bSpawning=false;
 bool CommitAcceptedShot(double Now);
 void MarkReady();bool Reject(const TCHAR* Reason);
};
