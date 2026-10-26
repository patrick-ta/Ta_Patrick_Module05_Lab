#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Week04HUD.generated.h"
UCLASS()
class COURSEGAME_API AWeek04HUD : public AHUD { GENERATED_BODY() public: virtual void DrawHUD() override; };
