#include "Week04/Week04HUD.h"
#include "Week04/Week04Controller.h"
#include "Week04/TargetCrate.h"
#include "Week04/HealthComponent.h"
#include "Week04/HealthObserver.h"
#include "Engine/Canvas.h"
void AWeek04HUD::DrawHUD()
{
    Super::DrawHUD(); if(!Canvas)return;
    auto* PC=Cast<AWeek04Controller>(GetOwningPlayerController());if(!PC)return;
    DrawRect(FLinearColor(0,0,0,.88f),0,0,Canvas->SizeX,158);
    float Y=8;auto Row=[&](const FString& Text){DrawText(Text,FLinearColor::White,12,Y);Y+=21;};
    Row(TEXT("WEEK 4 | 1: damage25 | 2: overkill500 | 3: -10 | 4: zero | 5: NaN | 6: infinity | 7: heal25"));
    Row(TEXT("T: fresh crate | O: attach/detach observer | K: destroy observer | P: player damage | R: respawn"));
    Row(TEXT("A/D: walk | Space: jump | E: interact | G: select | X: destroy selected | V: use cached target"));
    if(IsValid(PC->Crate))Row(FString::Printf(TEXT("CRATE health %.0f / %.0f | change events %d | death events %d | hidden %s"),PC->Crate->HealthComponent->GetHealth(),PC->Crate->HealthComponent->GetMaximum(),PC->Crate->ChangeEvents,PC->Crate->DeathEvents,PC->Crate->IsHidden()?TEXT("yes"):TEXT("no")));
    if(IsValid(PC->SecondObserver))Row(FString::Printf(TEXT("OBSERVER %s | changes %d | deaths %d | last health %.0f | snapshots %d"),PC->SecondObserver->IsAttached()?TEXT("attached"):TEXT("detached"),PC->SecondObserver->ChangeEvents,PC->SecondObserver->DeathEvents,PC->SecondObserver->LastHealth,PC->SecondObserver->Snapshots));
    else Row(TEXT("OBSERVER destroyed: model still owns health"));
    if(GetOwningPawn())if(auto* H=GetOwningPawn()->FindComponentByClass<UHealthComponent>())Row(FString::Printf(TEXT("PLAYER health %.0f | accepted interactions %d"),H->GetHealth(),PC->AcceptedInteractions));
    Row(PC->LastAction);
}
