#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ALCombatPrototypeHUD.generated.h"

UCLASS(Blueprintable)
class ASHLINEUI_API AALCombatPrototypeHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|UI") TSubclassOf<class UUserWidget> CombatWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|UI") FText BuildLabel = FText::FromString(TEXT("v0.0.1"));

    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetHealthDisplay(float CurrentHealth, float MaxHealth, float Delta);
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetAmmoDisplay(int32 MagazineAmmo, int32 ReserveAmmo);
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetEnemyCountDisplay(int32 DefeatedCount, int32 RemainingCount);
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetPrototypeCompleteDisplay();
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetBuildLabelDisplay(const FText& Label);

protected:
    UPROPERTY() TObjectPtr<class UUserWidget> CombatWidget;

    UFUNCTION() void HandleHealthChanged(float OldHealth, float NewHealth, float MaxHealth, float Delta);
    UFUNCTION() void HandleAmmoChanged(int32 MagazineAmmo, int32 ReserveAmmo);
    UFUNCTION() void HandleEnemyCountChanged(int32 DefeatedCount, int32 RemainingCount);
    UFUNCTION() void HandlePrototypeCompleted();
};
