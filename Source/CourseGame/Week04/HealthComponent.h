#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeek04HealthChanged,float,Health,float,Maximum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeek04Died);
UCLASS(ClassGroup=(Course),meta=(BlueprintSpawnableComponent))
class COURSEGAME_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UHealthComponent();
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable,Category="Health") void ApplyDamage(float Amount);
    UFUNCTION(BlueprintCallable,Category="Health") void Heal(float Amount);
    UFUNCTION(BlueprintPure,Category="Health") float GetHealth() const { return Health; }
    UFUNCTION(BlueprintPure,Category="Health") float GetMaximum() const { return MaxHealth; }
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Health") float MaxHealth=100.f;
    UPROPERTY(BlueprintAssignable,Category="Health") FWeek04HealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable,Category="Health") FWeek04Died OnDied;
private:
    UPROPERTY(VisibleAnywhere,Category="Health") float Health=100.f;
    bool bPublishing=false;
};
