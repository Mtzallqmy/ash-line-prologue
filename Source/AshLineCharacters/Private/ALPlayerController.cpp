#include "ALPlayerController.h"
#include "ALPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"

void AALPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ApplyPlayerMappingContext();
    SetMobileTouchEnabled(true);
}

void AALPlayerController::ApplyPlayerMappingContext()
{
    if (!PlayerMappingContext) return;
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer) return;
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        Subsystem->AddMappingContext(PlayerMappingContext, 0);
    }
}

void AALPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput) return;

    if (MoveAction) EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AALPlayerController::HandleMove);
    if (LookAction) EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AALPlayerController::HandleLook);
    if (JumpAction)
    {
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AALPlayerController::HandleJumpStarted);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AALPlayerController::HandleJumpCompleted);
    }
    if (CrouchAction) EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AALPlayerController::HandleCrouchStarted);
    if (SprintAction)
    {
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AALPlayerController::HandleSprintStarted);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AALPlayerController::HandleSprintCompleted);
    }
    if (InteractAction) EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AALPlayerController::HandleInteractStarted);
    if (PauseAction) EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &AALPlayerController::HandlePauseStarted);
    if (FireAction)
    {
        EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &AALPlayerController::HandleFireStarted);
        EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &AALPlayerController::HandleFireCompleted);
    }
    if (AimAction)
    {
        EnhancedInput->BindAction(AimAction, ETriggerEvent::Started, this, &AALPlayerController::HandleAimStarted);
        EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &AALPlayerController::HandleAimCompleted);
    }
    if (ReloadAction) EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &AALPlayerController::HandleReloadStarted);
    if (NextWeaponAction) EnhancedInput->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &AALPlayerController::HandleNextWeaponStarted);
}

AALPlayerCharacter* AALPlayerController::GetALPlayer() const
{
    return Cast<AALPlayerCharacter>(GetPawn());
}

void AALPlayerController::HandleMove(const FInputActionValue& Value) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->Move(Value); }
void AALPlayerController::HandleLook(const FInputActionValue& Value) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->Look(Value); }
void AALPlayerController::HandleJumpStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StartJump(); }
void AALPlayerController::HandleJumpCompleted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StopJump(); }
void AALPlayerController::HandleCrouchStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->ToggleCrouch(); }
void AALPlayerController::HandleSprintStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StartSprint(); }
void AALPlayerController::HandleSprintCompleted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StopSprint(); }
void AALPlayerController::HandleInteractStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->Interact(); }
void AALPlayerController::HandleFireStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StartFire(); }
void AALPlayerController::HandleFireCompleted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StopFire(); }
void AALPlayerController::HandleAimStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StartAim(); }
void AALPlayerController::HandleAimCompleted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->StopAim(); }
void AALPlayerController::HandleReloadStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->ReloadWeapon(); }
void AALPlayerController::HandleNextWeaponStarted(const FInputActionValue&) { if (AALPlayerCharacter* Player = GetALPlayer()) Player->SwitchWeapon(); }

void AALPlayerController::HandlePauseStarted(const FInputActionValue&)
{
    if (GetWorld()) UGameplayStatics::SetGamePaused(GetWorld(), !UGameplayStatics::IsGamePaused(GetWorld()));
}

void AALPlayerController::SetMobileTouchEnabled(bool bEnabled)
{
    bEnableTouchEvents = bEnabled;
    bEnableTouchOverEvents = bEnabled;
    if (bEnabled && MobileTouchWidgetClass && !MobileTouchWidget)
    {
        MobileTouchWidget = CreateWidget<UUserWidget>(this, MobileTouchWidgetClass);
        if (MobileTouchWidget) MobileTouchWidget->AddToViewport(20);
    }
    else if (!bEnabled && MobileTouchWidget)
    {
        MobileTouchWidget->RemoveFromParent();
        MobileTouchWidget = nullptr;
    }
}
