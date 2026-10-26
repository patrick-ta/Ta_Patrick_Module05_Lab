#include "Week05/CombatHUD.h"
#include "Week05/CombatController.h"
#include "Week05/BlasterComponent.h"
#include "Week04/TargetCrate.h"
#include "Week04/HealthComponent.h"
#include "Engine/Canvas.h"
void ACombatHUD::DrawHUD()
{
 Super::DrawHUD();if(!Canvas)return;
 auto* PC=Cast<ACombatController>(GetOwningPlayerController());if(!PC)return;
 DrawRect(FLinearColor(0,0,0,.9f),0,0,Canvas->SizeX,125);
 DrawText(TEXT("WEEK 5 | J: single shot | A/D: walk and face | Space: jump | T: reset range | V: 600/1800 speed | Tab: menu"),FLinearColor::White,12,12);
 if(PC->Weapon){auto* W=PC->Weapon.Get();DrawText(FString::Printf(TEXT("AMMO %d | accepted %d | rejected %d | speed %.0f | cooldown %.2fs | %s"),W->Ammo,W->Accepted,W->Rejected,W->ProjectileSpeed,W->Cooldown,*W->LastReason),FLinearColor::White,12,36);}
 if(IsValid(PC->Target)){auto* T=PC->Target.Get();DrawText(FString::Printf(TEXT("TARGET health %.0f | changes %d | deaths %d | hidden %s"),T->HealthComponent->GetHealth(),T->ChangeEvents,T->DeathEvents,T->IsHidden()?TEXT("yes"):TEXT("no")),FLinearColor::White,12,60);}
 DrawText(PC->bMenuOpen?TEXT("MENU OWNS INPUT: J must be rejected. Tab resumes gameplay."):TEXT("Aim at the crate. One accepted hit deals 25. T restores health and ammo. Misses expire after 2 game seconds."),FLinearColor::Yellow,12,84);
}
