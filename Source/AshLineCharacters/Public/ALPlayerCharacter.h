#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALMovementState.h"
#include "ALPlayerCharacter.generated.h"

struct FInputActionValue;

UCLASS(Blueprintable)
class ASHLINECHARACTERS_API AALPlayerCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AALPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Landed(const FHitResult& Hit) override;
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    virtual bool CanJumpInternal_Implementation() const override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StartJump();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StopJump();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StartSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StopSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void ToggleCrouch();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void Interact();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetMovementLocked(bool bLocked);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetLookLocked(bool bLocked);

    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") EALMovementState GetMovementState() const { return MovementState; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") float GetCurrentSpeed() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") float GetMovementDirection() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") bool IsSprinting() const { return bSprintActive; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") bool IsCrouching() const { return bIsCrouched; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Movement") bool IsAirborne() const { return GetCharacterMovement() && GetCharacterMovement()->IsFalling(); }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") TObjectPtr<USceneComponent> CameraRoot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") TObjectPtr<class UCameraComponent> FirstPersonCamera;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") TObjectPtr<class UALPlayerMovementSettings> MovementSettings;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALCombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALEquipmentComponent> EquipmentComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALPlayerStateComponent> PlayerStateComponent;

protected:
    void RefreshMovementState();
    void RefreshMovementSpeed();
    bool HasMovementSettings() const;
    bool bSprintIntent = false;
    bool bSprintActive = false;
    EALMovementState MovementState = EALMovementState::Walking;
    float CurrentPitch = 0.0f;
};
