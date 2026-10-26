#pragma once
#include "CoreMinimal.h"
#include "Gameplay/RunnerGameMode.h"
#include "Week04GameMode.generated.h"
UCLASS()
class COURSEGAME_API AWeek04GameMode : public ARunnerGameMode
{ GENERATED_BODY() public: AWeek04GameMode(); virtual void StartPlay() override; };
