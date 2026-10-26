#include "Week04/Week04Verification.h"
#include "Week04/HealthComponent.h"
#include "Week04/HealthObserver.h"
#include "Week04/TargetCrate.h"
#include "Week04/InteractionComponent.h"
#include "Week04/InteractionTargets.h"
#include "Week04/Week04Controller.h"
#include "Gameplay/RunnerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/GameViewportClient.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include <limits>
#include "Components/InputComponent.h"
AWeek04Verification::AWeek04Verification(){PrimaryActorTick.bCanEverTick=true;}
void AWeek04Verification::Check(const TCHAR* Name,bool Passed){bFailed|=!Passed;Results+=FString::Printf(TEXT("%s,%s\n"),Name,Passed?TEXT("PASS"):TEXT("FAIL"));UE_LOG(LogTemp,Display,TEXT("WEEK4_TEST %s %s"),Name,Passed?TEXT("PASS"):TEXT("FAIL"));}
void AWeek04Verification::Tick(float Dt)
{
    Super::Tick(Dt);Elapsed+=Dt;
    if(!bRan && Elapsed>3){bRan=true;RunTests();}
    const bool bCapture=FParse::Param(FCommandLine::Get(),TEXT("Week04Capture"));
    if(bCapture && bRan)
    {
        auto* PC=Cast<AWeek04Controller>(GetWorld()->GetFirstPlayerController());
        if(PC && IsValid(PC->Crate))
        {
            if(CaptureStep==0 && Elapsed>4.5f){PC->Crate->HealthComponent->ApplyDamage(25.f);FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Verification/Damage25.png"),false,false);++CaptureStep;}
            else if(CaptureStep==1 && Elapsed>6.f){PC->Crate->HealthComponent->ApplyDamage(500.f);FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Verification/Death.png"),false,false);++CaptureStep;}
        }
    }
    if(bRan && Elapsed>(bCapture?8.f:5.f))
    {
        IFileManager::Get().MakeDirectory(*(FPaths::ProjectSavedDir()/TEXT("Verification")),true);
        Results+=FString::Printf(TEXT("Overall,%s\n"),bFailed?TEXT("FAIL"):TEXT("PASS"));
        FFileHelper::SaveStringToFile(Results,*(FPaths::ProjectSavedDir()/TEXT("Verification/Week04.csv")));
        FPlatformMisc::RequestExitWithStatus(false,bFailed?1:0);SetActorTickEnabled(false);
    }
}
void AWeek04Verification::RunTests()
{
    auto* PC=Cast<AWeek04Controller>(GetWorld()->GetFirstPlayerController());
    auto* Runner=PC?Cast<ARunnerCharacter>(PC->GetPawn()):nullptr;
    Check(TEXT("Baseline_PossessedRunner"),Runner!=nullptr);
    Check(TEXT("Baseline_Grounded"),Runner && Runner->GetCharacterMovement()->IsMovingOnGround());
    Check(TEXT("Baseline_PlayerHealthComponent"),Runner && Runner->FindComponentByClass<UHealthComponent>()!=nullptr);
    Check(TEXT("Baseline_CrateInMap"),PC && IsValid(PC->Crate));
    UClass* BP=LoadClass<ATargetCrate>(nullptr,TEXT("/Game/Course/Week04/BP_TargetCrate.BP_TargetCrate_C"));
    Check(TEXT("Baseline_CrateBlueprintLoads"),BP!=nullptr);if(!BP)return;
    TArray<AActor*> Spawned;int32 N=0;
    FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto Crate=[&](){auto* C=GetWorld()->SpawnActor<ATargetCrate>(BP,FVector(10000+N++*200,0,100),FRotator::ZeroRotator,Params);Spawned.Add(C);return C;};
    auto Observer=[&](){auto* O=GetWorld()->SpawnActor<AHealthObserver>();Spawned.Add(O);return O;};
    ATargetCrate* C=Crate();UHealthComponent* H=C->HealthComponent;
    Check(TEXT("Baseline_HealthStarts100"),H->GetHealth()==100 && H->GetMaximum()==100);
    Check(TEXT("Baseline_NoStartupChangeOrDeath"),C->ChangeEvents==0 && C->DeathEvents==0);
    if(FParse::Param(FCommandLine::Get(),TEXT("LabBaseline")))
    {
        H->ApplyDamage(25);Check(TEXT("TODO_DamageRulesUnfinished"),H->GetHealth()==100 && C->ChangeEvents==0);
        C->ReceiveDeathVisual();Check(TEXT("TODO_BlueprintResponseUnfinished"),!C->IsHidden() && C->GetActorEnableCollision());
        auto* O=Observer();O->AttachToHealth(H);Check(TEXT("TODO_ObserverBindingUnfinished"),!O->IsAttached());
    }
    else
    {
        auto* O=Observer();O->AttachToHealth(H);
        Check(TEXT("Observer_InitialSnapshotNotChangeEvent"),O->Snapshots==1 && O->ChangeEvents==0 && O->LastHealth==100);
        H->ApplyDamage(25);
        Check(TEXT("AC1_100Minus25Is75"),H->GetHealth()==75);
        Check(TEXT("AC1_ExactlyOneChangeEvent"),C->ChangeEvents==1 && O->ChangeEvents==1 && O->LastHealth==75 && C->DeathEvents==0);
        H->ApplyDamage(500);
        Check(TEXT("AC2_OverkillClampedZero"),H->GetHealth()==0);
        Check(TEXT("AC2_ChangeAndDeathOnce"),C->ChangeEvents==2 && C->DeathEvents==1 && O->DeathEvents==1);
        Check(TEXT("Blueprint_DeathHidesCrate"),C->IsHidden());
        Check(TEXT("Blueprint_DeathDisablesCollision"),!C->GetActorEnableCollision());
        H->ApplyDamage(25);H->ApplyDamage(500);H->Heal(25);
        Check(TEXT("AC4_DeadDamageNoRepeatDeathOrChange"),C->DeathEvents==1 && C->ChangeEvents==2 && H->GetHealth()==0);
        Check(TEXT("Policy_HealingDoesNotRevive"),H->GetHealth()==0);
        ATargetCrate* Invalid=Crate();auto* IH=Invalid->HealthComponent.Get();
        const float Inputs[]={-10.f,0.f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity()};
        const TCHAR* Names[]={TEXT("AC3_NegativeRejected"),TEXT("AC3_ZeroRejected"),TEXT("AC3_NaNRejected"),TEXT("AC3_PositiveInfinityRejected"),TEXT("AC3_NegativeInfinityRejected")};
        for(int32 I=0;I<5;++I){IH->ApplyDamage(Inputs[I]);Check(Names[I],IH->GetHealth()==100 && Invalid->ChangeEvents==0 && Invalid->DeathEvents==0);}
        auto* A=Observer();auto* B=Observer();A->AttachToHealth(IH);B->AttachToHealth(IH);
        IH->ApplyDamage(25);Check(TEXT("Observer_TwoIndependentListeners"),A->ChangeEvents==1 && B->ChangeEvents==1);
        B->Detach();IH->ApplyDamage(25);
        Check(TEXT("AC5_DetachStopsOnlyItsCallbacks"),IH->GetHealth()==50 && A->ChangeEvents==2 && B->ChangeEvents==1 && !B->IsAttached());
        for(int32 I=0;I<5;++I)B->AttachToHealth(IH);
        IH->ApplyDamage(5);Check(TEXT("Observer_RepeatedAttachNoDuplicate"),B->ChangeEvents==2 && A->ChangeEvents==3);
        B->Destroy();IH->ApplyDamage(5);
        Check(TEXT("AC5_DestroyObserverHealthStillWorks"),IH->GetHealth()==40 && A->ChangeEvents==4);
        A->Destroy();IH->ApplyDamage(5);Check(TEXT("AC5_NoOptionalObserversRequired"),IH->GetHealth()==35);
        auto* New=Crate();auto* Switcher=Observer();Switcher->AttachToHealth(IH);Switcher->AttachToHealth(New->HealthComponent);
        IH->ApplyDamage(5);Check(TEXT("Observer_RebindDetachesOldSource"),Switcher->ChangeEvents==0);
        New->HealthComponent->ApplyDamage(25);Check(TEXT("Observer_RebindReceivesNewSource"),Switcher->ChangeEvents==1 && Switcher->LastHealth==75);
        New->Destroy();Switcher->Detach();Check(TEXT("Observer_DestroyedSourceSafeDetach"),!Switcher->IsAttached());
        auto* Reentrant=Crate();auto* Rec=Observer();Rec->AttachToHealth(Reentrant->HealthComponent);Rec->bReenterOnChange=true;
        Reentrant->HealthComponent->ApplyDamage(25);Check(TEXT("Model_RejectsRecursiveMutation"),Reentrant->HealthComponent->GetHealth()==75 && Rec->ChangeEvents==1);
        Reentrant->HealthComponent->Heal(500);Check(TEXT("Policy_HealingClampedToMaximum"),Reentrant->HealthComponent->GetHealth()==100 && Rec->ChangeEvents==2);
        // Direct reflected dispatch works on unrelated target classes; selection is exercised below.
    }
    // Interaction is supplied lecture scaffolding in both projects.
    auto* SelectionOwner=GetWorld()->SpawnActor<ATargetCrate>(FVector(30000,0,100),FRotator::ZeroRotator,Params);Spawned.Add(SelectionOwner);
    auto* Select=NewObject<UInteractionComponent>(SelectionOwner);SelectionOwner->AddInstanceComponent(Select);Select->RegisterComponent();Select->Range=160;
    auto* Near=GetWorld()->SpawnActor<ACourseDoor>(FVector(29920,0,100),FRotator::ZeroRotator,Params);Spawned.Add(Near);
    auto* Far=GetWorld()->SpawnActor<ACourseDoor>(FVector(29870,0,100),FRotator::ZeroRotator,Params);Spawned.Add(Far);
    Check(TEXT("Interaction_SelectsOneNearestReachable"),Select->SelectTarget()==Near);
    Check(TEXT("Interaction_ExecuteDispatchAcceptedOnce"),Select->DispatchSelected() && Near->Requests==1 && Far->Requests==0 && Select->AcceptedCount==1);
    Near->Destroy();Check(TEXT("Interaction_DestroyedCachedTargetSafe"),!Select->DispatchSelected() && Select->AcceptedCount==1);
    Far->Destroy();
    auto* TieA=GetWorld()->SpawnActor<AInteractionPickup>(FVector(29900,40,100),FRotator::ZeroRotator,Params);Spawned.Add(TieA);
    auto* TieB=GetWorld()->SpawnActor<AInteractionPickup>(FVector(29900,-40,100),FRotator::ZeroRotator,Params);Spawned.Add(TieB);
    AActor* Expected=TieA->GetPathName()<TieB->GetPathName()?static_cast<AActor*>(TieA):static_cast<AActor*>(TieB);
    Check(TEXT("Interaction_StableTieBreak"),Select->SelectTarget()==Expected);
    Check(TEXT("Interaction_PickupAcceptsOnce"),Select->DispatchSelected() && !Select->DispatchSelected());
    TieA->Destroy();TieB->Destroy();
    auto* Occluder=GetWorld()->SpawnActor<ATargetCrate>(FVector(29950,0,100),FRotator::ZeroRotator,Params);Spawned.Add(Occluder);
    auto* Blocked=GetWorld()->SpawnActor<ACourseDoor>(FVector(29900,0,100),FRotator::ZeroRotator,Params);Spawned.Add(Blocked);
    Check(TEXT("Interaction_BlockedTargetRejected"),Select->SelectTarget()==nullptr);
    Occluder->Destroy();Blocked->Destroy();
    for(AActor* Actor:Spawned)if(IsValid(Actor))Actor->Destroy();

    // Exercise the actual registered key delegates, without claiming OS keyboard testing.
    if(PC && IsValid(PC->Crate))
    {
        auto Press=[&](FKey Key){
            if(PC->InputComponent)for(const FInputKeyBinding& Binding:PC->InputComponent->KeyBindings)
                if(Binding.Chord.Key==Key && Binding.KeyEvent==IE_Pressed){Binding.KeyDelegate.Execute(Key);return true;}
            return false;
        };
        const bool Baseline=FParse::Param(FCommandLine::Get(),TEXT("LabBaseline"));
        Check(TEXT("Controls_T_ResetBound"),Press(EKeys::T) && PC->Crate->HealthComponent->GetHealth()==100);
        Check(TEXT("Controls_1_DamageBound"),Press(EKeys::One) && PC->Crate->HealthComponent->GetHealth()==(Baseline?100:75));
        if(!Baseline)
        {
            Press(EKeys::O);Press(EKeys::One);
            Check(TEXT("Controls_O_Detach"),PC->Crate->HealthComponent->GetHealth()==50 && PC->SecondObserver->ChangeEvents==1);
            Press(EKeys::O);Press(EKeys::One);
            Check(TEXT("Controls_O_Reattach"),PC->Crate->HealthComponent->GetHealth()==25 && PC->SecondObserver->ChangeEvents==2);
            Press(EKeys::K);Press(EKeys::One);
            Check(TEXT("Controls_K_DestroyObserver"),!IsValid(PC->SecondObserver) && PC->Crate->HealthComponent->GetHealth()==0 && PC->Crate->IsHidden());
        }
        Press(EKeys::T);Press(EKeys::Two);
        Check(TEXT("Controls_2_Overkill"),PC->Crate->HealthComponent->GetHealth()==(Baseline?100:0));
        Press(EKeys::One);Press(EKeys::Seven);
        Check(TEXT("Controls_DeadDamageHeal"),PC->Crate->DeathEvents==(Baseline?0:1) && PC->Crate->HealthComponent->GetHealth()==(Baseline?100:0));
        Press(EKeys::T);bool Bound=Press(EKeys::Three);Bound=Press(EKeys::Four)&&Bound;Bound=Press(EKeys::Five)&&Bound;Bound=Press(EKeys::Six)&&Bound;
        Check(TEXT("Controls_InvalidRequests"),Bound && PC->Crate->HealthComponent->GetHealth()==100 && PC->Crate->ChangeEvents==0);
        Press(EKeys::P);
        APawn* Previous=PC->GetPawn();
        Check(TEXT("Controls_P_PlayerDamage"),Previous && Previous->FindComponentByClass<UHealthComponent>()->GetHealth()==(Baseline?100:75));
        Check(TEXT("Controls_R_Respawn"),Press(EKeys::R) && PC->GetPawn()!=Previous && PC->GetPawn()->FindComponentByClass<UHealthComponent>()->GetHealth()==100);
        const int32 Count=PC->PlayerObserver->ChangeEvents;Press(EKeys::P);
        Check(TEXT("Controls_PlayerObserverFollowsRespawn"),PC->PlayerObserver->ChangeEvents==Count+(Baseline?0:1) && PC->GetPawn()->FindComponentByClass<UHealthComponent>()->GetHealth()==(Baseline?100:75));
        Press(EKeys::R);Press(EKeys::T);
    }

    if(FParse::Param(FCommandLine::Get(),TEXT("Week04Capture")))
        FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Verification/Week04.png"),false,false);
}

