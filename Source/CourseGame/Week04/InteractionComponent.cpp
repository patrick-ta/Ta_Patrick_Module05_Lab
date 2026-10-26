#include "Week04/InteractionComponent.h"
#include "Week04/Interactable.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
UInteractionComponent::UInteractionComponent(){PrimaryComponentTick.bCanEverTick=false;}
bool UInteractionComponent::CanReach(AActor* Target) const
{
    if(!IsValid(Target)||!GetOwner()||!Target->Implements<UInteractable>())return false;
    const FVector Delta=Target->GetActorLocation()-GetOwner()->GetActorLocation();
    // Course camera: screen-forward/right is world -X. Zero distance is allowed.
    if(Delta.SizeSquared()>FMath::Square(Range))return false;
    if(!Delta.IsNearlyZero() && FVector::DotProduct(Delta.GetSafeNormal(),FVector(-1,0,0))<.25f)return false;
    FHitResult Hit;FCollisionQueryParams Params(SCENE_QUERY_STAT(Week04Visibility),false,GetOwner());
    const bool Blocked=GetWorld()->LineTraceSingleByChannel(Hit,GetOwner()->GetActorLocation(),Target->GetActorLocation(),ECC_Visibility,Params);
    return !Blocked || Hit.GetActor()==Target;
}
AActor* UInteractionComponent::SelectTarget()
{
    Selected.Reset();if(!GetOwner())return nullptr;
    TArray<FOverlapResult> Hits;FCollisionQueryParams Params(SCENE_QUERY_STAT(Week04Candidates),false,GetOwner());
    GetWorld()->OverlapMultiByObjectType(Hits,GetOwner()->GetActorLocation(),FQuat::Identity,FCollisionObjectQueryParams::AllObjects,FCollisionShape::MakeSphere(Range),Params);
    TSet<AActor*> Seen;float Best=TNumericLimits<float>::Max();
    for(const auto& Hit:Hits)
    {
        AActor* A=Hit.GetActor();if(Seen.Contains(A)||!CanReach(A))continue;Seen.Add(A);
        const float D=FVector::DistSquared(A->GetActorLocation(),GetOwner()->GetActorLocation());
        if(D<Best || (D==Best && (!Selected.IsValid()||A->GetPathName()<Selected->GetPathName()))){Best=D;Selected=A;}
    }
    return Selected.Get();
}
bool UInteractionComponent::DispatchSelected()
{
    AActor* Target=Selected.Get();
    if(!GetOwner()||!GetOwner()->HasAuthority()||!CanReach(Target))return false;
    if(!IInteractable::Execute_Interact(Target,GetOwner()))return false;
    ++AcceptedCount;OnAccepted.Broadcast(IsValid(Target)?Target:nullptr);return true;
}
