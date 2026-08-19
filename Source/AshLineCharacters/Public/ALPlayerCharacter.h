#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALMovementState.h"
#include "ALPlayerCharacter.generated.h"

struct FInputActionValue;
class AALWeaponBase;
class UALWeaponDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALPlayerDeathFlowEvent);

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

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Death") void RequestRestartFromCheckpoint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Death") void ApplyCheckpointRestart(const FTransform& CheckpointTransform);
    UFUNCTION(BlueprintPure, Category="ASH LINE|Death") bool IsDeathFlowActive() const { return bDeathFlowActive; }

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Death") FALPlayerDeathFlowEvent OnPlayerDeathFlowStarted;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Death") FALPlayerDeathFlowEvent OnRestartCheckpointRequested;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StartJump();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StopJump();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StartSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StopSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void ToggleCrouch();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void Interact();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void ReloadWeapon();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SwitchWeapon();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SpawnDefaultLoadout();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void EquipDevelopmentSMG();
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALEquipmentComponent> EquipmentComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALPlayerStateComponent> PlayerStateComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") TObjectPtr<class UALWeaponComponent> WeaponComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout") TSubclassOf<AALWeaponBase> PrimaryWeaponClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout") TObjectPtr<UALWeaponDataAsset> PrimaryWeaponData;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout") TSubclassOf<AALWeaponBase> SidearmWeaponClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout") TObjectPtr<UALWeaponDataAsset> SidearmWeaponData;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout|Development") TSubclassOf<AALWeaponBase> DevelopmentSMGClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Loadout|Development") TObjectPtr<UALWeaponDataAsset> DevelopmentSMGData;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Loadout") TObjectPtr<AALWeaponBase> PrimaryWeaponInstance;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Loadout") TObjectPtr<AALWeaponBase> SidearmWeaponInstance;

protected:
    AALWeaponBase* SpawnLoadoutWeapon(TSubclassOf<AALWeaponBase> WeaponClass, UALWeaponDataAsset* WeaponData, bool bPrimarySlot);
    UFUNCTION() void HandleHealthDeath();
    void HandleFallDamage();
    void SetAimFOV(bool bAiming);
    void UpdateAimFOV();
    void RefreshMovementState();
    void RefreshMovementSpeed();
    bool HasMovementSettings() const;
    bool bSprintIntent = false;
    bool bSprintActive = false;
    EALMovementState MovementState = EALMovementState::Walking;
    float CurrentPitch = 0.0f;
    bool bDeathFlowActive = false;
    float LastFallSpeed = 0.0f;
    float CurrentFOV = 90.0f;
    float TargetFOV = 90.0f;
    float AimFOVTransitionTime = 0.15f;
    FTimerHandle DeathRestartTimer;
    FTimerHandle AimFOVTimer;
};
