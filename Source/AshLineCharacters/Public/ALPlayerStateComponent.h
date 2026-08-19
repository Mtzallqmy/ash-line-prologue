#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALPlayerStateComponent.generated.h"

UENUM(BlueprintType)
enum class EALPlayerControlBlock : uint8
{
    None,
    Scripted,
    Dialogue,
    Drone,
    Pause
};

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALPlayerStateComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetMovementEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetLookEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetInteractionEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetInputLocked(bool bLocked);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Control") void SetControlBlock(EALPlayerControlBlock NewBlock);

    UFUNCTION(BlueprintPure, Category="ASH LINE|Control") bool CanMove() const { return bCanMove && !bInputLocked; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Control") bool CanLook() const { return bCanLook && !bInputLocked; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Control") bool CanInteract() const { return bCanInteract && !bInputLocked; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Control") bool IsInputLocked() const { return bInputLocked; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Control") EALPlayerControlBlock GetControlBlock() const { return ControlBlock; }

    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Control") bool bCanMove = true;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Control") bool bCanLook = true;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Control") bool bCanInteract = true;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Control") bool bInputLocked = false;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Control") EALPlayerControlBlock ControlBlock = EALPlayerControlBlock::None;
};
