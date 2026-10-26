#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"
UINTERFACE(BlueprintType)
class COURSEGAME_API UInteractable : public UInterface { GENERATED_BODY() };
class COURSEGAME_API IInteractable
{
    GENERATED_BODY()
public:
    // Return true only when this target accepts the request.
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Interaction") bool Interact(AActor* Interactor);
};
