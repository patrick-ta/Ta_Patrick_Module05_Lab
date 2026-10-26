#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Week04AssetLibrary.generated.h"
class UBlueprint;
UCLASS()
class COURSEGAMEEDITOR_API UWeek04AssetLibrary : public UBlueprintFunctionLibrary
{ GENERATED_BODY() public: UFUNCTION(BlueprintCallable) static bool ConfigureCrateBlueprint(UBlueprint* Blueprint,bool bSolution); };
