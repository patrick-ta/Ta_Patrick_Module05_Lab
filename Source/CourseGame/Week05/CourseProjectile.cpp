#include "Week05/CourseProjectile.h"
#include "Week04/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
ACourseProjectile::ACourseProjectile()
{
 PrimaryActorTick.bCanEverTick=false;
 Collision=CreateDefaultSubobject<USphereComponent>(TEXT("CollisionRoot"));SetRootComponent(Collision);
 Collision->InitSphereRadius(3.f);Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
 Collision->SetGenerateOverlapEvents(false);Collision->SetNotifyRigidBodyCollision(true);
 Visual=CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ProjectileSprite"));Visual->SetupAttachment(Collision);
 Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);Visual->SetRelativeScale3D(FVector(.25f));
 Movement=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
 Movement->SetUpdatedComponent(Collision);Movement->InitialSpeed=600.f;Movement->MaxSpeed=600.f;
 Movement->ProjectileGravityScale=0.f;Movement->bSweepCollision=true;
 Movement->SetPlaneConstraintNormal(FVector(0,1,0));Movement->SetPlaneConstraintEnabled(true);
 Collision->OnComponentHit.AddDynamic(this,&ACourseProjectile::OnHit);
}
void ACourseProjectile::Initialize(APawn* Shooter,float InDamage,float Speed,float Facing)
{
 SetOwner(Shooter);SetInstigator(Shooter);Damage=InDamage;
 if(Shooter)Collision->IgnoreActorWhenMoving(Shooter,true);
 Movement->InitialSpeed=Speed;Movement->MaxSpeed=Speed;
 Movement->Velocity=FVector(Facing<0?-Speed:Speed,0,0);
}
void ACourseProjectile::BeginPlay()
{
 Super::BeginPlay();
 // LAB 4: give missed projectiles a finite positive lifetime.
 const float SafeLifetime=(FMath::IsFinite(Lifetime) && Lifetime>0.f)?Lifetime:2.f;
 SetLifeSpan(SafeLifetime);
}
void ACourseProjectile::ConsumeImpact(AActor* Other)
{
 // LAB 3: reject unauthorized, self and repeated impacts.
 // Consume BEFORE callbacks. Disable collision, stop movement, apply damage once, destroy.
 if(!HasAuthority())return;
 if(bConsumed)return;
 if(Other && (Other==GetOwner() || Other==GetInstigator()))return;

 bConsumed=true;
 ++ImpactCount;

 Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 Movement->StopMovementImmediately();
 Movement->Deactivate();

 if(Other)
 {
  if(UHealthComponent* H=Other->FindComponentByClass<UHealthComponent>())
  {
   H->ApplyDamage(Damage);
  }
 }

 Destroy();
}
void ACourseProjectile::OnHit(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,FVector,const FHitResult&){ConsumeImpact(Other);}
void ACourseProjectile::EndPlay(const EEndPlayReason::Type Reason)
{
 Collision->OnComponentHit.RemoveDynamic(this,&ACourseProjectile::OnHit);
 Super::EndPlay(Reason);
}
