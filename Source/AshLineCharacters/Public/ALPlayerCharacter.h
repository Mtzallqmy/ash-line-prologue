#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class ASHLINECHARACTERS_API AALPlayerCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AALPlayerCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void MoveForward(float Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void MoveRight(float Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void LookYaw(float Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void LookPitch(float Value);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StartSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void StopSprint();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void ToggleCrouch();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void Interact();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void Fire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void Reload();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALCombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALEquipmentComponent> EquipmentComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALPlayerStateComponent> PlayerStateComponent;
};
