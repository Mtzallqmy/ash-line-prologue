#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALPlayerMovementSettings.generated.h"

UENUM(BlueprintType)
enum class EALSprintMode : uint8
{
    Hold,
    Toggle
};

USTRUCT(BlueprintType)
struct FALPlayerInputSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Look") float HorizontalSensitivity = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Look") float VerticalSensitivity = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Look") bool bInvertY = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mobile") EALSprintMode SprintMode = EALSprintMode::Hold;
};

UCLASS(BlueprintType)
class ASHLINECHARACTERS_API UALPlayerMovementSettings : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float WalkSpeed = 320.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float SprintSpeed = 520.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float CrouchSpeed = 180.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float JumpVelocity = 420.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera") float MinPitch = -85.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera") float MaxPitch = 85.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction") float InteractionDistance = 250.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input") FALPlayerInputSettings Input;
};
