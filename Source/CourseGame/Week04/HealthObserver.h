#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthObserver.generated.h"
class UHealthComponent;
UCLASS(Blueprintable)
class COURSEGAME_API AHealthObserver : public AActor
{
    GENERATED_BODY()
public:
    AHealthObserver();
    UFUNCTION(BlueprintCallable,Category="Observer") void AttachToHealth(UHealthComponent* NewSource);
    UFUNCTION(BlueprintCallable,Category="Observer") void Detach();
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 ChangeEvents=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 DeathEvents=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 Snapshots=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) float LastHealth=0;
    bool IsAttached() const { return Source.IsValid(); }
    // Opt-in test hook proves event callbacks cannot recursively apply damage.
    bool bReenterOnChange=false;
private:
    UPROPERTY() TWeakObjectPtr<UHealthComponent> Source;
    UFUNCTION() void Changed(float Health,float Maximum);
    UFUNCTION() void Died();
};
