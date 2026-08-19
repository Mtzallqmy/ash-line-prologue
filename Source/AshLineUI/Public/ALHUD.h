#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ALHUD.generated.h"

UCLASS(Blueprintable)
class ASHLINEUI_API AALHUD : public AHUD
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void ShowInteractionPrompt(const FText& Prompt);
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetMissionObjective(const FText& Objective);
    UFUNCTION(BlueprintImplementableEvent, Category="ASH LINE|UI") void SetDroneMode(bool bEnabled);
};
