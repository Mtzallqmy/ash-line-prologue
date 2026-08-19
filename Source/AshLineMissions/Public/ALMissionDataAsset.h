#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALMissionDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FALMissionObjective
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ObjectiveId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;
};

UCLASS(BlueprintType)
class ASHLINEMISSIONS_API UALMissionDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FALMissionObjective> Objectives;
};
