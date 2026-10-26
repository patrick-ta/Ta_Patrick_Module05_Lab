#include "Week05/BlasterComponent.h"
#include "Week05/BlasterRules.h"
#include "Week05/CourseProjectile.h"
#include "Week04/HealthComponent.h"
#include "Gameplay/RunnerCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
UBlasterComponent::UBlasterComponent(){PrimaryComponentTick.bCanEverTick=false;ProjectileClass=ACourseProjectile::StaticClass();}
bool UBlasterComponent::CanFire(double Now) const
{
 const auto* Owner=Cast<ARunnerCharacter>(GetOwner());
 const auto* H=Owner?Owner->FindComponentByClass<UHealthComponent>():nullptr;
 return Owner && Owner->HasAuthority() && !bSpawning && BlasterRules::CanFire(Now,NextAllowedFireTime,H && H->GetHealth()>0,Owner->bGameplayEnabled,Ammo)
     && ProjectileClass && FMath::IsFinite(Cooldown) && Cooldown>0 && FMath::IsFinite(Damage) && Damage>0 && FMath::IsFinite(ProjectileSpeed) && ProjectileSpeed>0;
}
bool UBlasterComponent::Reject(const TCHAR* Reason){++Rejected;LastReason=Reason;UE_LOG(LogTemp,Display,TEXT("W5_FIRE_REJECTED %s"),Reason);return false;}
bool UBlasterComponent::TryFire()
{
 const double Now=GetWorld()->GetTimeSeconds();
 if(!CanFire(Now))return Reject(TEXT("Invalid state, ammo, class, settings or cooldown"));
 auto* Shooter=Cast<ARunnerCharacter>(GetOwner());
 const float Facing=Shooter->FacingX;
 const FVector Start=Shooter->GetActorLocation();const FVector Muzzle=Start+FVector(Facing*18.f,0,0);
 FHitResult Hit;FCollisionQueryParams Query(SCENE_QUERY_STAT(BlasterMuzzle),false,Shooter);
 if(GetWorld()->SweepSingleByChannel(Hit,Start,Muzzle,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(3.f),Query))return Reject(TEXT("Muzzle blocked"));
 TGuardValue<bool> Guard(bSpawning,true);
 const FTransform SpawnTransform(FRotator::ZeroRotator,Muzzle);
 auto* Shot=GetWorld()->SpawnActorDeferred<ACourseProjectile>(ProjectileClass,SpawnTransform,Shooter,Shooter,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
 if(!Shot)return Reject(TEXT("Spawn failed"));
 Shot->Initialize(Shooter,Damage,ProjectileSpeed,Facing);
 UGameplayStatics::FinishSpawningActor(Shot,SpawnTransform);
 if(!IsValid(Shot))return Reject(TEXT("Projectile invalid after spawn"));
 if(!CommitAcceptedShot(Now)){Shot->Destroy();return Reject(TEXT("LAB: commit not implemented"));}
 LastProjectile=Shot;bReady=false;LastReason=TEXT("Accepted");
 // This timer is cosmetic readiness only. The timestamp remains the authority.
 GetWorld()->GetTimerManager().SetTimer(ReadyTimer,this,&UBlasterComponent::MarkReady,Cooldown,false);
 OnShotAccepted.Broadcast();UE_LOG(LogTemp,Display,TEXT("W5_FIRE_ACCEPTED count=%d ammo=%d next=%.6f"),Accepted,Ammo,NextAllowedFireTime);
 return true;
}
bool UBlasterComponent::CommitAcceptedShot(double Now)
{
 // LAB 2: this function is called only after a valid projectile spawned.
 // Spend one round, advance the next permitted time, increment Accepted.
 if(!FMath::IsFinite(Now) || Ammo<=0 || !FMath::IsFinite(Cooldown) || Cooldown<=0)return false;
 --Ammo;
 NextAllowedFireTime=Now+Cooldown;
 ++Accepted;
 return false;
}
void UBlasterComponent::MarkReady(){bReady=true;}
bool UBlasterComponent::HasReadyTimer() const{return GetWorld() && GetWorld()->GetTimerManager().TimerExists(ReadyTimer);}
void UBlasterComponent::EndPlay(const EEndPlayReason::Type Reason)
{
 // LAB 5: clear ReadyTimer before completing EndPlay.
 OnShotAccepted.Clear();Super::EndPlay(Reason);
}
