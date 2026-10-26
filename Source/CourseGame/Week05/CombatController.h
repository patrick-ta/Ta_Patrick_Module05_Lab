#pragma once
#include "CoreMinimal.h"
#include "Gameplay/RunnerController.h"
#include "CombatController.generated.h"
class UBlasterComponent;class ATargetCrate;
UCLASS()
class COURSEGAME_API ACombatController:public ARunnerController
{
 GENERATED_BODY()
public:
 virtual void SetupInputComponent() override;
 virtual void OnPossess(APawn* InPawn) override;
 UPROPERTY() TObjectPtr<UBlasterComponent> Weapon;
 UPROPERTY() TObjectPtr<ATargetCrate> Target;
 void Fire();void ResetRange();void ToggleSpeed();
};
