#include "ALPlayerCharacter.h"
#include "ALHealthComponent.h"
#include "ALCharacterComponents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

AALPlayerCharacter::AALPlayerCharacter()
{
    HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent"));
    CombatComponent = CreateDefaultSubobject<UALCombatComponent>(TEXT("CombatComponent"));
    InventoryComponent = CreateDefaultSubobject<UALInventoryComponent>(TEXT("InventoryComponent"));
    EquipmentComponent = CreateDefaultSubobject<UALEquipmentComponent>(TEXT("EquipmentComponent"));
    InteractionComponent = CreateDefaultSubobject<UALInteractionComponent>(TEXT("InteractionComponent"));
    PlayerStateComponent = CreateDefaultSubobject<UALPlayerStateComponent>(TEXT("PlayerStateComponent"));
    GetCharacterMovement()->MaxWalkSpeed = 420.0f;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 180.0f;
}

void AALPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AALPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AALPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("LookYaw"), this, &AALPlayerCharacter::LookYaw);
    PlayerInputComponent->BindAxis(TEXT("LookPitch"), this, &AALPlayerCharacter::LookPitch);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AALPlayerCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AALPlayerCharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AALPlayerCharacter::ToggleCrouch);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AALPlayerCharacter::Interact);
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AALPlayerCharacter::Fire);
    PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AALPlayerCharacter::Reload);
}

void AALPlayerCharacter::MoveForward(float Value) { if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorForwardVector(), Value); }
void AALPlayerCharacter::MoveRight(float Value) { if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) AddMovementInput(GetActorRightVector(), Value); }
void AALPlayerCharacter::LookYaw(float Value) { AddControllerYawInput(Value); }
void AALPlayerCharacter::LookPitch(float Value) { AddControllerPitchInput(Value); }
void AALPlayerCharacter::StartSprint() { GetCharacterMovement()->MaxWalkSpeed = 620.0f; }
void AALPlayerCharacter::StopSprint() { GetCharacterMovement()->MaxWalkSpeed = 420.0f; }
void AALPlayerCharacter::ToggleCrouch() { if (bIsCrouched) UnCrouch(); else Crouch(); }
void AALPlayerCharacter::Interact() { if (InteractionComponent) InteractionComponent->FindInteractable(); }
void AALPlayerCharacter::Fire() { if (CombatComponent) CombatComponent->FireCurrentWeapon(); }
void AALPlayerCharacter::Reload() { if (CombatComponent) CombatComponent->ReloadCurrentWeapon(); }
