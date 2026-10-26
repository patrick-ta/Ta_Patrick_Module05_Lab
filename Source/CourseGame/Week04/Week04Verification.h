#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Week04Verification.generated.h"
UCLASS()
class COURSEGAME_API AWeek04Verification : public AActor
{
    GENERATED_BODY()
public:
    AWeek04Verification(); virtual void Tick(float DeltaSeconds) override;
private:
    int32 CaptureStep=0;float Elapsed=0;bool bRan=false,bFailed=false;FString Results=TEXT("check,result\n");
    void Check(const TCHAR* Name,bool Passed);void RunTests();
};
