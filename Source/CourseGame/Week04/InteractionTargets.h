#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Week04/Interactable.h"
#include "InteractionTargets.generated.h"
class UStaticMeshComponent;
UCLASS(Blueprintable)
class COURSEGAME_API ACourseDoor : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    ACourseDoor();
    virtual bool Interact_Implementation(AActor* Interactor) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) bool bOpen=false;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 Requests=0;
};
UCLASS(Blueprintable)
class COURSEGAME_API AInteractionPickup : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    AInteractionPickup();
    virtual bool Interact_Implementation(AActor* Interactor) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) bool bCollected=false;
};
UCLASS(Blueprintable)
class COURSEGAME_API ARechargeStation : public AActor, public IInteractable
{
    GENERATED_BODY()
public:
    ARechargeStation();
    virtual bool Interact_Implementation(AActor* Interactor) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> Mesh;
};
