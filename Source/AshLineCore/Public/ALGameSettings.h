#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ALGameSettings.generated.h"

UENUM(BlueprintType)
enum class EALGraphicsProfile : uint8 { Low, Medium, High };

UCLASS(config=Game, defaultconfig, BlueprintType)
class ASHLINECORE_API UALGameSettings : public UDeveloperSettings
{
    GENERATED_BODY()
public:
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 HardDownloadBudgetMB = 500;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 CoreRuntimeBudgetMB = 150;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 PrologueCityBudgetMB = 170;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 CharactersBudgetMB = 60;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 WeaponsBudgetMB = 30;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 AudioBudgetMB = 40;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 UIBudgetMB = 15;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 DroneVFXBudgetMB = 15;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Build Budget") int32 ReservedOverheadMB = 20;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance") int32 MaxActiveEnemies = 10;
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance") EALGraphicsProfile DefaultGraphicsProfile = EALGraphicsProfile::Medium;
};
