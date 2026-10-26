#include "Week05/CombatVerification.h"
#include "Week05/BlasterComponent.h"
#include "Week05/BlasterRules.h"
#include "Week05/CombatController.h"
#include "Gameplay/RunnerCharacter.h"
#include "Week04/TargetCrate.h"
#include "Week04/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include <limits>
ACombatVerification::ACombatVerification(){PrimaryActorTick.bCanEverTick=true;}
void ACombatVerification::Check(const TCHAR* Name,bool Passed){Failed|=!Passed;Results+=FString::Printf(TEXT("%s,%s\n"),Name,Passed?TEXT("PASS"):TEXT("FAIL"));UE_LOG(LogTemp,Display,TEXT("W5_TEST %s %s"),Name,Passed?TEXT("PASS"):TEXT("FAIL"));}
void ACombatVerification::Reenter(float,float){if(ReentrantProjectile)ReentrantProjectile->ConsumeImpact(SweepTargets[0]);}
void ACombatVerification::Tick(float Dt)
{
 Super::Tick(Dt);Elapsed+=Dt;
 if(Phase==0 && Elapsed>3){StartTests();Elapsed=0;Phase=1;}
 else if(Phase==1 && Elapsed>3){FinishTests();Phase=2;Elapsed=0;}
 else if(Phase==2 && Elapsed>1){FPlatformMisc::RequestExitWithStatus(false,Failed?1:0);SetActorTickEnabled(false);}
}
void ACombatVerification::StartTests()
{
 Baseline=FParse::Param(FCommandLine::Get(),TEXT("LabBaseline"));
 auto* PC=Cast<ACombatController>(GetWorld()->GetFirstPlayerController());
 Check(TEXT("Baseline_CombatController"),PC && PC->Weapon);
 Check(TEXT("Baseline_TargetAndWorkingHealth"),PC && IsValid(PC->Target) && PC->Target->HealthComponent->GetHealth()==100);
 if(!PC || !PC->Weapon || !IsValid(PC->Target))return;
 if(PC && PC->Target){PC->Target->HealthComponent->ApplyDamage(25);Check(TEXT("Baseline_Week4HealthAlreadyComplete"),PC->Target->HealthComponent->GetHealth()==75);PC->ResetRange();}
 Check(TEXT("Rule_FirstAtZero"),BlasterRules::CanFire(0,0,true,true,1)==!Baseline);
 Check(TEXT("Rule_0249Rejected"),!BlasterRules::CanFire(.249,.250,true,true,1));
 Check(TEXT("Rule_0250Accepted"),BlasterRules::CanFire(.250,.250,true,true,1)==!Baseline);
 Check(TEXT("Rule_BeforeBoundaryRejected"),!BlasterRules::CanFire(.249999,.250,true,true,1));
 Check(TEXT("Rule_AfterBoundaryAccepted"),BlasterRules::CanFire(.251,.250,true,true,1)==!Baseline);
 Check(TEXT("Rule_DeadRejected"),!BlasterRules::CanFire(1,0,false,true,1));
 Check(TEXT("Rule_MenuRejected"),!BlasterRules::CanFire(1,0,true,false,1));
 Check(TEXT("Rule_EmptyAmmoRejected"),!BlasterRules::CanFire(1,0,true,true,0));
 Check(TEXT("Rule_NonFiniteClockRejected"),!BlasterRules::CanFire(std::numeric_limits<double>::quiet_NaN(),0,true,true,1));
 FActorSpawnParameters P;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 Shooter=GetWorld()->SpawnActor<ARunnerCharacter>(FVector(10000,0,500),FRotator::ZeroRotator,P);Fixtures.Add(Shooter);
 Check(TEXT("Baseline_TestShooterSpawned"),IsValid(Shooter));
 if(!IsValid(Shooter))return;
 Shooter->GetCharacterMovement()->DisableMovement();
 auto* W=NewObject<UBlasterComponent>(Shooter);Shooter->AddInstanceComponent(W);W->RegisterComponent();
 W->ProjectileClass=LoadClass<ACourseProjectile>(nullptr,TEXT("/Game/Course/Week05/BP_Projectile.BP_Projectile_C"));
 Check(TEXT("Baseline_ProjectileBlueprintLoads"),W->ProjectileClass!=nullptr);
 if(!W->ProjectileClass)return;
 auto Target=[&](FVector At){auto* T=GetWorld()->SpawnActor<ATargetCrate>(At,FRotator::ZeroRotator,P);Fixtures.Add(T);return T;};
 auto Shot=[&](FVector At,float Speed){FTransform X(FRotator::ZeroRotator,At);auto* S=GetWorld()->SpawnActorDeferred<ACourseProjectile>(W->ProjectileClass,X,Shooter,Shooter,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);S->Initialize(Shooter,25,Speed,-1);UGameplayStatics::FinishSpawningActor(S,X);Fixtures.Add(S);return S;};
 auto* T=Target(FVector(9800,0,500));
 const bool First=W->TryFire();Check(TEXT("Fire_FirstRequest"),First==!Baseline);
 if(!Baseline && First)
 {
  auto* S=W->LastProjectile.Get();Check(TEXT("Fire_CommitsAmmoAndCount"),W->Ammo==19 && W->Accepted==1);
  const double Next=W->NextAllowedFireTime;
  Check(TEXT("Fire_ImmediateDuplicateRejected"),!W->TryFire() && W->Ammo==19 && W->Accepted==1 && W->NextAllowedFireTime==Next);
  Check(TEXT("Projectile_OwnerAndInstigator"),S && S->GetOwner()==Shooter && S->GetInstigator()==Shooter);
  Check(TEXT("Projectile_CollisionRootAndUpdatedComponent"),S && S->GetRootComponent()==S->Collision && S->Movement->UpdatedComponent==S->Collision);
  Check(TEXT("Projectile_SweptMovementEnabled"),S && S->Movement->bSweepCollision);
  S->ConsumeImpact(Shooter);Check(TEXT("Impact_ShooterImmune"),Shooter->FindComponentByClass<UHealthComponent>()->GetHealth()==100 && !S->bConsumed);
  S->ConsumeImpact(T);S->ConsumeImpact(T);
  Check(TEXT("Impact_DuplicateAppliesOnce"),T->HealthComponent->GetHealth()==75 && T->ChangeEvents==1 && S->ImpactCount==1);
  Check(TEXT("Impact_DisablesCollisionImmediately"),S->Collision->GetCollisionEnabled()==ECollisionEnabled::NoCollision);
  Check(TEXT("Impact_StopsMovement"),S->Movement->Velocity.IsNearlyZero());
  Check(TEXT("Impact_DestroysProjectile"),!IsValid(S));
  W->NextAllowedFireTime=0;W->ProjectileClass=AFailedSpawnProjectile::StaticClass();
  Check(TEXT("Fire_FailedSpawnDoesNotCommit"),!W->TryFire() && W->Ammo==19 && W->Accepted==1 && W->NextAllowedFireTime==0);
  W->ProjectileClass=LoadClass<ACourseProjectile>(nullptr,TEXT("/Game/Course/Week05/BP_Projectile.BP_Projectile_C"));
  W->Ammo=0;Check(TEXT("Fire_NoAmmoRejected"),!W->TryFire());W->Ammo=20;
  Shooter->SetGameplayEnabled(false);Check(TEXT("Fire_MenuGateRejected"),!W->TryFire());Shooter->SetGameplayEnabled(true);
  auto* Block=Target(FVector(9985,0,500));
  Check(TEXT("Fire_BlockedMuzzleDoesNotCommit"),!W->TryFire() && W->Ammo==20 && W->NextAllowedFireTime==0);Block->Destroy();
  auto* RT=Target(FVector(11000,0,500));SweepTargets.Add(RT);
  ReentrantProjectile=Shot(FVector(11100,0,500),600);
  RT->HealthComponent->OnHealthChanged.AddDynamic(this,&ACombatVerification::Reenter);
  ReentrantProjectile->ConsumeImpact(RT);
  Check(TEXT("Impact_ReentrantCallbackConsumedFirst"),ReentrantProjectile->ImpactCount==1 && RT->HealthComponent->GetHealth()==75);
  RT->HealthComponent->OnHealthChanged.RemoveDynamic(this,&ACombatVerification::Reenter);
  SweepTargets.Empty();
 }
 else if(Baseline)
 {
  auto* S=Shot(FVector(11500,0,500),600);S->ConsumeImpact(T);
  Check(TEXT("TODO_ImpactUnfinished"),T->HealthComponent->GetHealth()==100 && !S->bConsumed);S->Destroy();
  Check(TEXT("TODO_FireUnfinished"),W->Ammo==20 && W->Accepted==0);
 }
 // Actual world movement through a thin wall, and into a target, at both speeds.
 for(int32 I=0;I<4;++I)
 {
  const float Z=1000+I*200;
  auto* Victim=Target(FVector(19800,0,Z));SweepTargets.Add(Victim);
  if(I>=2)
  {
   auto* Wall=GetWorld()->SpawnActor<AActor>();Fixtures.Add(Wall);
   auto* Box=NewObject<UBoxComponent>(Wall);Wall->SetRootComponent(Box);Wall->AddInstanceComponent(Box);
   Box->SetBoxExtent(FVector(2,20,60));Box->SetCollisionProfileName(TEXT("BlockAll"));Box->RegisterComponent();Wall->SetActorLocation(FVector(19900,0,Z));
  }
  SweepShots.Add(Shot(FVector(20000,0,Z),I%2?1800:600));
 }
 Miss=Shot(FVector(30000,0,3000),600);
 Check(TEXT("Lifetime_Configured"),Baseline?Miss->GetLifeSpan()==0:Miss->GetLifeSpan()>0 && Miss->GetLifeSpan()<=2.01f);
 // Timer cleanup is checked immediately after component EndPlay.
 if(!Baseline)
 {
  W->NextAllowedFireTime=0;W->TryFire();Check(TEXT("Timer_StartedForAcceptedShot"),W->HasReadyTimer());
  const FTimerHandle EndedTimer=W->GetReadyTimerHandle();
  EndedWeapon=W;W->DestroyComponent();Check(TEXT("Timer_ClearedOnEndPlay"),!GetWorld()->GetTimerManager().TimerExists(EndedTimer));
 }
 if(PC && PC->Weapon)
 {
  for(const auto& Binding:PC->InputComponent->KeyBindings)if(Binding.Chord.Key==EKeys::J && Binding.KeyEvent==IE_Pressed){Binding.KeyDelegate.Execute(EKeys::J);break;}
  Check(TEXT("Input_JRegisteredSingleRequest"),PC->Weapon->Accepted==(Baseline?0:1) && PC->Weapon->Rejected==(Baseline?1:0));
 }
}
void ACombatVerification::FinishTests()
{
 for(int32 I=0;I<SweepTargets.Num();++I)
 {
  const bool Direct=I<2;
  Check(*FString::Printf(TEXT("World_%s_%dSpeed"),Direct?TEXT("TargetHit"):TEXT("ThinWallBlocks"),I%2?1800:600),SweepTargets[I]->HealthComponent->GetHealth()==(!Baseline && Direct?75:100));
  if(!Baseline)Check(*FString::Printf(TEXT("World_ProjectileConsumed_%d"),I),!IsValid(SweepShots[I]));
 }
 Check(TEXT("Lifetime_MissExpires"),Baseline?IsValid(Miss):!IsValid(Miss));
 auto* PC=Cast<ACombatController>(GetWorld()->GetFirstPlayerController());
 if(PC && !Baseline)Check(TEXT("World_DemoKeyShotDamagedCrate"),PC->Target && PC->Target->HealthComponent->GetHealth()==75);
 for(const auto& A:Fixtures)if(IsValid(A))A->Destroy();
 const FString Dir=FPaths::ProjectSavedDir()/TEXT("Verification");IFileManager::Get().MakeDirectory(*Dir,true);
 Results+=FString::Printf(TEXT("Overall,%s\n"),Failed?TEXT("FAIL"):TEXT("PASS"));
 FFileHelper::SaveStringToFile(Results,*(Dir/TEXT("Week05.csv")));
 if(FParse::Param(FCommandLine::Get(),TEXT("Week05Capture")))FScreenshotRequest::RequestScreenshot(Dir/TEXT("Week05.png"),false,false);
}

