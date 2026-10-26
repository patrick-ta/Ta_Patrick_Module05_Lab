#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetCrate.generated.h"
class UHealthComponent;
class UStaticMeshComponent;
UCLASS(Blueprintable)
class COURSEGAME_API ATargetCrate : public AActor
{
    GENERATED_BODY()
public:
    ATargetCrate();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Crate") TObjectPtr<UHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Crate") TObjectPtr<UStaticMeshComponent> CrateMesh;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Evidence") int32 ChangeEvents=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Evidence") int32 DeathEvents=0;
    UFUNCTION(BlueprintImplementableEvent,Category="Crate") void ReceiveDeathVisual();
private:
    UFUNCTION() void HealthChanged(float Health,float Maximum);
    UFUNCTION() void HandleDeath();
};
