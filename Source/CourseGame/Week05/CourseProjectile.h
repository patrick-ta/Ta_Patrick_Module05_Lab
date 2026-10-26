#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CourseProjectile.generated.h"
class USphereComponent;class UPaperSpriteComponent;class UProjectileMovementComponent;
UCLASS(Blueprintable)
class COURSEGAME_API ACourseProjectile:public AActor
{
 GENERATED_BODY()
public:
 ACourseProjectile();
 virtual void BeginPlay() override;
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<USphereComponent> Collision;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UPaperSpriteComponent> Visual;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UProjectileMovementComponent> Movement;
 UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Blaster") float Lifetime=2.f;
 UPROPERTY(BlueprintReadOnly,Category="Blaster") bool bConsumed=false;
 UPROPERTY(BlueprintReadOnly,Category="Blaster") int32 ImpactCount=0;
 float Damage=25.f;
 void Initialize(APawn* Shooter,float InDamage,float Speed,float Facing);
 UFUNCTION(BlueprintCallable,Category="Blaster") void ConsumeImpact(AActor* Other);
private:
 UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent,AActor* Other,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit);
};
