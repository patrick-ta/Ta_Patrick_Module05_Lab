#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeek04InteractionAccepted,AActor*,Target);
UCLASS(ClassGroup=(Course),meta=(BlueprintSpawnableComponent))
class COURSEGAME_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UInteractionComponent();
    UFUNCTION(BlueprintCallable) AActor* SelectTarget();
    UFUNCTION(BlueprintCallable) bool DispatchSelected();
    AActor* GetSelected() const {return Selected.Get();}
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Range=160.f;
    UPROPERTY(BlueprintAssignable) FWeek04InteractionAccepted OnAccepted;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 AcceptedCount=0;
private:
    bool CanReach(AActor* Target) const;
    UPROPERTY() TWeakObjectPtr<AActor> Selected;
};
