#pragma once
#include "CoreMinimal.h"
#include "Gameplay/RunnerGameMode.h"
#include "CombatGameMode.generated.h"
UCLASS() class COURSEGAME_API ACombatGameMode:public ARunnerGameMode
{GENERATED_BODY() public:ACombatGameMode();virtual void StartPlay() override;};
