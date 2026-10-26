#include "Week04/Week04GameMode.h"
#include "Week04/Week04Controller.h"
#include "Week04/Week04HUD.h"
#include "Week04/Week04Verification.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Engine/World.h"
AWeek04GameMode::AWeek04GameMode(){PlayerControllerClass=AWeek04Controller::StaticClass();HUDClass=AWeek04HUD::StaticClass();}
void AWeek04GameMode::StartPlay(){Super::StartPlay();if(FParse::Param(FCommandLine::Get(),TEXT("Week04Verify")))GetWorld()->SpawnActor<AWeek04Verification>();}
