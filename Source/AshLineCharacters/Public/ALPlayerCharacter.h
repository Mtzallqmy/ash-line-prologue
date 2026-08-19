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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALCombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALEquipmentComponent> EquipmentComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALPlayerStateComponent> PlayerStateComponent;
};
