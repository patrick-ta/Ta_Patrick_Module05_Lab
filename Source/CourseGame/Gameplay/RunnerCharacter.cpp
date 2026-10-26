#include "Gameplay/RunnerCharacter.h"
#include "Week04/InteractionComponent.h"
#include "Week04/HealthComponent.h"
#include "Gameplay/RunnerCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
ARunnerCharacter::ARunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CreateDefaultSubobject<UHealthComponent>(TEXT("PlayerHealth"));
    CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
    GetCapsuleComponent()->InitCapsuleSize(10.f,16.f);
    GetSprite()->SetRelativeLocation(FVector(0,-2,-16));
    GetSprite()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bUseControllerRotationPitch=bUseControllerRotationYaw=bUseControllerRotationRoll=false;
    auto* Movement=GetCharacterMovement();
    Movement->bOrientRotationToMovement=false;
    Movement->SetPlaneConstraintNormal(FVector(0,1,0));
    Movement->SetPlaneConstraintOrigin(FVector::ZeroVector);
    Movement->SetPlaneConstraintEnabled(true);
    Movement->bSnapToPlaneAtStart=true;
    Movement->MaxWalkSpeed=120.f;
    Movement->MaxAcceleration=720.f;
    Movement->BrakingDecelerationWalking=900.f;
    Movement->JumpZVelocity=230.f;
    Movement->GravityScale=1.f;
    Movement->AirControl=0.35f;
    JumpMaxHoldTime=0.20f;
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> Idle(TEXT("/Game/Course/Characters/FB_PlayerIdle"));
    static ConstructorHelpers::FObjectFinder<UPaperFlipbook> Run(TEXT("/Game/Course/Characters/FB_PlayerRun"));
    IdleAnimation=Idle.Object; RunAnimation=Run.Object;
    GetSprite()->SetFlipbook(IdleAnimation);
}
void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    auto* Enhanced=Cast<UEnhancedInputComponent>(Input);
    if (!ensure(Enhanced && MoveAction && JumpAction)) return;
    Enhanced->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ARunnerCharacter::Move);
    Enhanced->BindAction(MoveAction,ETriggerEvent::Completed,this,&ARunnerCharacter::ReleaseMove);
    Enhanced->BindAction(MoveAction,ETriggerEvent::Canceled,this,&ARunnerCharacter::ReleaseMove);
    Enhanced->BindAction(JumpAction,ETriggerEvent::Started,this,&ARunnerCharacter::BeginJump);
    Enhanced->BindAction(JumpAction,ETriggerEvent::Completed,this,&ARunnerCharacter::EndJump);
    Enhanced->BindAction(JumpAction,ETriggerEvent::Canceled,this,&ARunnerCharacter::EndJump);
}
void ARunnerCharacter::ApplyMoveIntent(float Value)
{
    if (!bGameplayEnabled) return;
    // Camera is at negative Y looking toward +Y: screen right is WORLD -X.
    // CharacterMovement integrates time; the correct path does not multiply by dt.
    float Scale=FMath::Clamp(Value,-1.f,1.f);
    if(FMath::Abs(Scale)>.01f)FacingX=Scale>0?-1.f:1.f;
    if (bDemonstrateDeltaTimeBug) Scale*=GetWorld()->GetDeltaSeconds();
    AddMovementInput(FVector(-1,0,0),Scale);
}
void ARunnerCharacter::Move(const FInputActionValue& Value) { ApplyMoveIntent(Value.Get<float>()); }
void ARunnerCharacter::ReleaseMove(const FInputActionValue&) { ConsumeMovementInputVector(); }
void ARunnerCharacter::BeginJump() { if (bGameplayEnabled) Jump(); }
void ARunnerCharacter::EndJump() { StopJumping(); }
void ARunnerCharacter::SetGameplayEnabled(bool bEnabled)
{
    bGameplayEnabled=bEnabled;
    StopJumping(); ConsumeMovementInputVector();
    if (!bEnabled) GetCharacterMovement()->StopMovementImmediately();
}
bool ARunnerCharacter::ProbeFloor()
{
    const FVector Start=GetActorLocation();
    const FVector End=Start-FVector(0,0,GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+12.f);
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(Week3FloorProbe),false,this);
    bFloorProbeHit=GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
    if (bShowFloorProbe)
    {
        DrawDebugLine(GetWorld(),Start,End,bFloorProbeHit?FColor::Green:FColor::Red,false,0.f,0,1.f);
        if (bFloorProbeHit) DrawDebugPoint(GetWorld(),Hit.ImpactPoint,5.f,FColor::Yellow,false,0.f);
    }
    return bFloorProbeHit;
}
void ARunnerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    ProbeFloor(); // Diagnostic only. CharacterMovement still owns grounding.
    auto* Animation=FMath::Abs(GetVelocity().X)>2.f?RunAnimation.Get():IdleAnimation.Get();
    if (Animation && GetSprite()->GetFlipbook()!=Animation) GetSprite()->SetFlipbook(Animation);
}

