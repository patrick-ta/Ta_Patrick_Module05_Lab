#include "Week04/Week04Controller.h"
#include "Week04/HealthComponent.h"
#include "Week04/HealthObserver.h"
#include "Week04/TargetCrate.h"
#include "Week04/InteractionComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include <limits>
void AWeek04Controller::BeginPlay()
{
    Super::BeginPlay();
    CrateClass=LoadClass<ATargetCrate>(nullptr,TEXT("/Game/Course/Week04/BP_TargetCrate.BP_TargetCrate_C"));
    for(TActorIterator<ATargetCrate> It(GetWorld());It;++It) { Crate=*It; break; }
    SecondObserver=GetWorld()->SpawnActor<AHealthObserver>();
    if(Crate) SecondObserver->AttachToHealth(Crate->HealthComponent);
}
void AWeek04Controller::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::One,IE_Pressed,this,&AWeek04Controller::Damage25);
    InputComponent->BindKey(EKeys::Two,IE_Pressed,this,&AWeek04Controller::Overkill);
    InputComponent->BindKey(EKeys::Three,IE_Pressed,this,&AWeek04Controller::Negative);
    InputComponent->BindKey(EKeys::Four,IE_Pressed,this,&AWeek04Controller::Zero);
    InputComponent->BindKey(EKeys::Five,IE_Pressed,this,&AWeek04Controller::NotANumber);
    InputComponent->BindKey(EKeys::Six,IE_Pressed,this,&AWeek04Controller::Infinity);
    InputComponent->BindKey(EKeys::Seven,IE_Pressed,this,&AWeek04Controller::Heal25);
    InputComponent->BindKey(EKeys::O,IE_Pressed,this,&AWeek04Controller::ToggleObserver);
    InputComponent->BindKey(EKeys::K,IE_Pressed,this,&AWeek04Controller::DestroyObserver);
    InputComponent->BindKey(EKeys::T,IE_Pressed,this,&AWeek04Controller::ResetCrate);
    InputComponent->BindKey(EKeys::P,IE_Pressed,this,&AWeek04Controller::DamagePlayer);
    InputComponent->BindKey(EKeys::E,IE_Pressed,this,&AWeek04Controller::SelectAndInteract);
    InputComponent->BindKey(EKeys::G,IE_Pressed,this,&AWeek04Controller::CacheTarget);
    InputComponent->BindKey(EKeys::V,IE_Pressed,this,&AWeek04Controller::DispatchCached);
    InputComponent->BindKey(EKeys::X,IE_Pressed,this,&AWeek04Controller::DestroyCached);
}
void AWeek04Controller::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if(!IsValid(PlayerObserver)) PlayerObserver=GetWorld()->SpawnActor<AHealthObserver>();
    PlayerObserver->AttachToHealth(InPawn?InPawn->FindComponentByClass<UHealthComponent>():nullptr);
}
void AWeek04Controller::EndPlay(const EEndPlayReason::Type Reason)
{
    if(IsValid(SecondObserver)) SecondObserver->Destroy();
    if(IsValid(PlayerObserver)) PlayerObserver->Destroy();
    Super::EndPlay(Reason);
}
void AWeek04Controller::Damage(float Amount,const TCHAR* Label)
{
    if(IsValid(Crate)) Crate->HealthComponent->ApplyDamage(Amount);
    LastAction=Label;
}
void AWeek04Controller::Damage25(){Damage(25.f,TEXT("Damage request: 25"));}
void AWeek04Controller::Overkill(){Damage(500.f,TEXT("Damage request: 500"));}
void AWeek04Controller::Negative(){Damage(-10.f,TEXT("Damage request: -10 (reject)"));}
void AWeek04Controller::Zero(){Damage(0.f,TEXT("Damage request: 0 (reject)"));}
void AWeek04Controller::NotANumber(){Damage(std::numeric_limits<float>::quiet_NaN(),TEXT("Damage request: NaN (reject)"));}
void AWeek04Controller::Infinity(){Damage(std::numeric_limits<float>::infinity(),TEXT("Damage request: infinity (reject)"));}
void AWeek04Controller::Heal25(){if(IsValid(Crate))Crate->HealthComponent->Heal(25.f);LastAction=TEXT("Heal request: 25; no resurrection");}
void AWeek04Controller::ResetCrate()
{
    if(IsValid(SecondObserver))SecondObserver->Destroy();
    if(IsValid(Crate))Crate->Destroy();
    FActorSpawnParameters Params; Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Crate=GetWorld()->SpawnActor<ATargetCrate>(CrateClass,FVector(420,0,34),FRotator::ZeroRotator,Params);
    SecondObserver=GetWorld()->SpawnActor<AHealthObserver>();
    if(Crate)SecondObserver->AttachToHealth(Crate->HealthComponent);
    LastAction=TEXT("Fresh crate and observer: health 100, event counts 0");
}
void AWeek04Controller::ToggleObserver()
{
    if(!IsValid(SecondObserver))SecondObserver=GetWorld()->SpawnActor<AHealthObserver>();
    if(SecondObserver->IsAttached()){SecondObserver->Detach();LastAction=TEXT("Second observer detached");}
    else {SecondObserver->AttachToHealth(IsValid(Crate)?Crate->HealthComponent.Get():nullptr);LastAction=TEXT("Second observer attached with snapshot");}
}
void AWeek04Controller::DestroyObserver(){if(IsValid(SecondObserver))SecondObserver->Destroy();SecondObserver=nullptr;LastAction=TEXT("Second observer destroyed; health must still work");}
void AWeek04Controller::DamagePlayer(){if(GetPawn())if(auto* H=GetPawn()->FindComponentByClass<UHealthComponent>())H->ApplyDamage(25.f);LastAction=TEXT("Player uses the same health component");}
UInteractionComponent* AWeek04Controller::GetInteraction() const {return GetPawn()?GetPawn()->FindComponentByClass<UInteractionComponent>():nullptr;}
void AWeek04Controller::SelectAndInteract(){if(auto* I=GetInteraction()){I->SelectTarget();if(I->DispatchSelected())++AcceptedInteractions;LastAction=TEXT("E: one selected target requested");}}
void AWeek04Controller::CacheTarget(){if(auto* I=GetInteraction()){I->SelectTarget();LastAction=TEXT("G: cached optional target");}}
void AWeek04Controller::DispatchCached(){if(auto* I=GetInteraction()){const bool OK=I->DispatchSelected();if(OK)++AcceptedInteractions;LastAction=OK?TEXT("V: cached target accepted"):TEXT("V: no valid reachable target; safely ignored");}}
void AWeek04Controller::DestroyCached(){if(auto* I=GetInteraction())if(auto* Target=I->GetSelected())Target->Destroy();LastAction=TEXT("X: selected target destroyed");}
