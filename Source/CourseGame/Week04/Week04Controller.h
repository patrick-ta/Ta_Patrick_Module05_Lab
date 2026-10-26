#pragma once
#include "CoreMinimal.h"
#include "Gameplay/RunnerController.h"
#include "Week04Controller.generated.h"
class ATargetCrate;
class AHealthObserver;
class UInteractionComponent;
UCLASS()
class COURSEGAME_API AWeek04Controller : public ARunnerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UPROPERTY() TObjectPtr<ATargetCrate> Crate;
    UPROPERTY() TObjectPtr<AHealthObserver> SecondObserver;
    UPROPERTY() TObjectPtr<AHealthObserver> PlayerObserver;
    UPROPERTY() TSubclassOf<ATargetCrate> CrateClass;
    FString LastAction=TEXT("Ready: 1 applies 25 damage; T resets the crate");
    int32 AcceptedInteractions=0;
    void ResetCrate();
private:
    void Damage25(); void Overkill(); void Negative(); void Zero(); void NotANumber(); void Infinity(); void Heal25();
    void ToggleObserver(); void DestroyObserver(); void DamagePlayer();
    void SelectAndInteract(); void CacheTarget(); void DispatchCached(); void DestroyCached();
    void Damage(float Amount,const TCHAR* Label);
    UInteractionComponent* GetInteraction() const;
};
