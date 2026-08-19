#include "ALPlayerCharacter.h"
#include "Components/ALHealthComponent.h"
#include "ALCharacterComponents.h"
#include "ALInteractionComponent.h"
#include "ALPlayerMovementSettings.h"
#include "ALPlayerStateComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"

namespace ALPlayerDefaults
{
    constexpr float WalkSpeed = 320.0f;
    constexpr float SprintSpeed = 520.0f;
    constexpr float CrouchSpeed = 180.0f;
    constexpr float JumpVelocity = 420.0f;
    constexpr float MinPitch = -85.0f;
    constexpr float MaxPitch = 85.0f;
    constexpr float DeathRestartDelay = 1.5f;
    constexpr float FallMinimumSpeed = 900.0f;
    constexpr float FallFatalSpeed = 2200.0f;
    constexpr float FallMaxDamage = 100.0f;
}

AALPlayerCharacter::AALPlayerCharacter()
{
    CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
    CameraRoot->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(CameraRoot);
    FirstPersonCamera->bUsePawnControlRotation = true;

    HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent"));
    CombatComponent = CreateDefaultSubobject<UALCombatComponent>(TEXT("CombatComponent"));
    InventoryComponent = CreateDefaultSubobject<UALInventoryComponent>(TEXT("InventoryComponent"));
    EquipmentComponent = CreateDefaultSubobject<UALEquipmentComponent>(TEXT("EquipmentComponent"));
    InteractionComponent = CreateDefaultSubobject<UALInteractionComponent>(TEXT("InteractionComponent"));
    PlayerStateComponent = CreateDefaultSubobject<UALPlayerStateComponent>(TEXT("PlayerStateComponent"));

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AALPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (MovementSettings)
    {
        GetCharacterMovement()->JumpZVelocity = MovementSettings->JumpVelocity;
        GetCharacterMovement()->MaxWalkSpeedCrouched = MovementSettings->CrouchSpeed;
        InteractionComponent->InteractionRange = MovementSettings->InteractionDistance;
        CurrentPitch = FMath::Clamp(GetControlRotation().Pitch, MovementSettings->MinPitch, MovementSettings->MaxPitch);
    }
    else
    {
        GetCharacterMovement()->JumpZVelocity = ALPlayerDefaults::JumpVelocity;
        GetCharacterMovement()->MaxWalkSpeedCrouched = ALPlayerDefaults::CrouchSpeed;
        CurrentPitch = FMath::Clamp(GetControlRotation().Pitch, ALPlayerDefaults::MinPitch, ALPlayerDefaults::MaxPitch);
    }
    if (HealthComponent) HealthComponent->OnDeath.AddDynamic(this, &AALPlayerCharacter::HandleHealthDeath);
    RefreshMovementSpeed();
    RefreshMovementState();
}

bool AALPlayerCharacter::HasMovementSettings() const
{
    return IsValid(MovementSettings);
}

void AALPlayerCharacter::Move(const FInputActionValue& Value)
{
    if (!PlayerStateComponent || !PlayerStateComponent->CanMove()) return;
    const FVector2D Input = Value.Get<FVector2D>().GetClampedToMaxSize(1.0f);
    if (Input.IsNearlyZero()) return;

    const FRotator ControlRotation = Controller ? Controller->GetControlRotation() : GetActorRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(Forward, Input.Y);
    AddMovementInput(Right, Input.X);
}

void AALPlayerCharacter::Look(const FInputActionValue& Value)
{
    if (!PlayerStateComponent || !PlayerStateComponent->CanLook()) return;
    const FVector2D Input = Value.Get<FVector2D>();
    const FALPlayerInputSettings InputSettings = MovementSettings ? MovementSettings->Input : FALPlayerInputSettings();
    const float YawDelta = Input.X * InputSettings.HorizontalSensitivity;
    const float PitchSign = InputSettings.bInvertY ? 1.0f : -1.0f;
    const float PitchDelta = Input.Y * InputSettings.VerticalSensitivity * PitchSign;
    const float MinPitch = MovementSettings ? MovementSettings->MinPitch : ALPlayerDefaults::MinPitch;
    const float MaxPitch = MovementSettings ? MovementSettings->MaxPitch : ALPlayerDefaults::MaxPitch;

    if (Controller)
    {
        FRotator Rotation = Controller->GetControlRotation();
        CurrentPitch = FMath::Clamp(CurrentPitch + PitchDelta, MinPitch, MaxPitch);
        Rotation.Pitch = CurrentPitch;
        Rotation.Yaw += YawDelta;
        Controller->SetControlRotation(Rotation);
    }
}

void AALPlayerCharacter::StartJump()
{
    if (PlayerStateComponent && PlayerStateComponent->CanMove() && CanJump()) Jump();
}

void AALPlayerCharacter::StopJump()
{
    StopJumping();
}

void AALPlayerCharacter::StartSprint()
{
    if (!PlayerStateComponent || !PlayerStateComponent->CanMove() || bIsCrouched || IsAirborne()) return;
    bSprintIntent = true;
    bSprintActive = true;
    RefreshMovementState();
    RefreshMovementSpeed();
}

void AALPlayerCharacter::StopSprint()
{
    bSprintIntent = false;
    bSprintActive = false;
    RefreshMovementState();
    RefreshMovementSpeed();
}

void AALPlayerCharacter::ToggleCrouch()
{
    if (!PlayerStateComponent || !PlayerStateComponent->CanMove()) return;
    if (bIsCrouched) UnCrouch();
    else { StopSprint(); Crouch(); }
    RefreshMovementState();
    RefreshMovementSpeed();
}

void AALPlayerCharacter::Interact()
{
    if (PlayerStateComponent && PlayerStateComponent->CanInteract() && InteractionComponent) InteractionComponent->TryInteract();
}

void AALPlayerCharacter::SetMovementLocked(bool bLocked)
{
    if (PlayerStateComponent) PlayerStateComponent->SetMovementEnabled(!bLocked);
    if (bLocked)
    {
        StopSprint();
        GetCharacterMovement()->StopMovementImmediately();
    }
    RefreshMovementState();
}

void AALPlayerCharacter::SetLookLocked(bool bLocked)
{
    if (PlayerStateComponent) PlayerStateComponent->SetLookEnabled(!bLocked);
}

bool AALPlayerCharacter::CanJumpInternal_Implementation() const
{
    return PlayerStateComponent && PlayerStateComponent->CanMove() && MovementState != EALMovementState::Locked && Super::CanJumpInternal_Implementation();
}

void AALPlayerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    HandleFallDamage();
    RefreshMovementState();
}

void AALPlayerCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    if (PreviousMovementMode == MOVE_Falling) LastFallSpeed = FMath::Abs(GetVelocity().Z);
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
    RefreshMovementState();
}

void AALPlayerCharacter::HandleFallDamage()
{
    if (!HealthComponent || HealthComponent->IsDead()) return;
    const FALFallDamageSettings Settings = HealthComponent->HealthConfig ? HealthComponent->HealthConfig->FallDamage : FALFallDamageSettings();
    const float MinimumSpeed = HealthComponent->HealthConfig ? Settings.MinimumFallSpeed : ALPlayerDefaults::FallMinimumSpeed;
    const float FatalSpeed = HealthComponent->HealthConfig ? Settings.FatalFallSpeed : ALPlayerDefaults::FallFatalSpeed;
    const float MaxDamage = HealthComponent->HealthConfig ? Settings.MaxFallDamage : ALPlayerDefaults::FallMaxDamage;
    const float LandingSpeed = LastFallSpeed;
    LastFallSpeed = 0.0f;
    if (LandingSpeed <= MinimumSpeed || FatalSpeed <= MinimumSpeed || MaxDamage <= 0.0f) return;

    const float Alpha = FMath::Clamp((LandingSpeed - MinimumSpeed) / (FatalSpeed - MinimumSpeed), 0.0f, 1.0f);
    FALDamageData DamageData;
    DamageData.BaseDamage = FMath::Lerp(0.0f, MaxDamage, Alpha);
    DamageData.DamageType = EALDamageType::Fall;
    DamageData.DamageCauser = this;
    DamageData.HitLocation = GetActorLocation();
    DamageData.HitNormal = FVector::UpVector;
    DamageData.bHasHitResult = true;
    HealthComponent->ApplyDamage(DamageData);
}

void AALPlayerCharacter::HandleHealthDeath()
{
    if (bDeathFlowActive) return;
    bDeathFlowActive = true;
    SetMovementLocked(true);
    SetLookLocked(true);
    if (PlayerStateComponent)
    {
        PlayerStateComponent->SetInteractionEnabled(false);
        PlayerStateComponent->SetInputLocked(true);
    }
    if (InteractionComponent) InteractionComponent->SetInteractionEnabled(false);
    OnPlayerDeathFlowStarted.Broadcast();
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(DeathRestartTimer, this, &AALPlayerCharacter::RequestRestartFromCheckpoint, ALPlayerDefaults::DeathRestartDelay, false);
}

void AALPlayerCharacter::RequestRestartFromCheckpoint()
{
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(DeathRestartTimer);
    if (HealthComponent) HealthComponent->ResetHealth();
    bDeathFlowActive = false;
    if (PlayerStateComponent)
    {
        PlayerStateComponent->SetControlBlock(EALPlayerControlBlock::None);
        PlayerStateComponent->SetInputLocked(false);
    }
    if (InteractionComponent) InteractionComponent->SetInteractionEnabled(true);
    SetLookLocked(false);
    SetMovementLocked(false);
    OnRestartCheckpointRequested.Broadcast();
}

void AALPlayerCharacter::RefreshMovementState()
{
    if (!PlayerStateComponent || !PlayerStateComponent->CanMove()) MovementState = EALMovementState::Locked;
    else if (GetCharacterMovement()->IsFalling()) MovementState = EALMovementState::Airborne;
    else if (bIsCrouched) MovementState = EALMovementState::Crouching;
    else if (bSprintActive) MovementState = EALMovementState::Sprinting;
    else MovementState = EALMovementState::Walking;
}

void AALPlayerCharacter::RefreshMovementSpeed()
{
    const float WalkSpeed = MovementSettings ? MovementSettings->WalkSpeed : ALPlayerDefaults::WalkSpeed;
    const float SprintSpeed = MovementSettings ? MovementSettings->SprintSpeed : ALPlayerDefaults::SprintSpeed;
    const float CrouchSpeed = MovementSettings ? MovementSettings->CrouchSpeed : ALPlayerDefaults::CrouchSpeed;
    GetCharacterMovement()->MaxWalkSpeed = bIsCrouched ? CrouchSpeed : (bSprintActive ? SprintSpeed : WalkSpeed);
    GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

float AALPlayerCharacter::GetCurrentSpeed() const
{
    return GetVelocity().Size2D();
}

float AALPlayerCharacter::GetMovementDirection() const
{
    const FVector HorizontalVelocity = FVector(GetVelocity().X, GetVelocity().Y, 0.0f);
    if (HorizontalVelocity.IsNearlyZero()) return 0.0f;
    return FMath::RadiansToDegrees(FMath::Atan2(HorizontalVelocity | GetActorRightVector(), HorizontalVelocity | GetActorForwardVector()));
}
