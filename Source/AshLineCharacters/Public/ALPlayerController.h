#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ALPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(Blueprintable)
class ASHLINECHARACTERS_API AALPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputMappingContext> PlayerMappingContext;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> MoveAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> JumpAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> CrouchAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> SprintAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> InteractAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Input") TObjectPtr<UInputAction> PauseAction;

    // Reserved for Prompt 03/04 integrations; these actions are not bound in Prompt 02.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Future Input") TObjectPtr<UInputAction> FireAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Future Input") TObjectPtr<UInputAction> AimAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Future Input") TObjectPtr<UInputAction> ReloadAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Future Input") TObjectPtr<UInputAction> DroneAction;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Weapon Input") TObjectPtr<UInputAction> NextWeaponAction;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void ApplyPlayerMappingContext();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Input") void SetMobileTouchEnabled(bool bEnabled);

protected:
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void HandleJumpStarted(const FInputActionValue& Value);
    void HandleJumpCompleted(const FInputActionValue& Value);
    void HandleCrouchStarted(const FInputActionValue& Value);
    void HandleSprintStarted(const FInputActionValue& Value);
    void HandleSprintCompleted(const FInputActionValue& Value);
    void HandleInteractStarted(const FInputActionValue& Value);
    void HandlePauseStarted(const FInputActionValue& Value);
    void HandleFireStarted(const FInputActionValue& Value);
    void HandleFireCompleted(const FInputActionValue& Value);
    void HandleAimStarted(const FInputActionValue& Value);
    void HandleAimCompleted(const FInputActionValue& Value);
    void HandleReloadStarted(const FInputActionValue& Value);
    void HandleNextWeaponStarted(const FInputActionValue& Value);

    class AALPlayerCharacter* GetALPlayer() const;
};
