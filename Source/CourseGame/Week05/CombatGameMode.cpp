#include "Week05/CombatGameMode.h"
#include "Week05/CombatController.h"
#include "Week05/CombatHUD.h"
#include "Week05/CombatVerification.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
ACombatGameMode::ACombatGameMode(){PlayerControllerClass=ACombatController::StaticClass();HUDClass=ACombatHUD::StaticClass();}
void ACombatGameMode::StartPlay(){Super::StartPlay();if(FParse::Param(FCommandLine::Get(),TEXT("Week05Verify")))GetWorld()->SpawnActor<ACombatVerification>();}
