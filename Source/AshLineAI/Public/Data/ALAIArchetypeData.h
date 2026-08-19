#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALAIArchetypeData.generated.h"

class UALHealthConfig;
class UALWeaponDataAsset;

UENUM(BlueprintType)
enum class EALAIAccuracy : uint8
{
    Basic,
    Trained
};

USTRUCT(BlueprintType)
struct ASHLINEAI_API FALBurstSettings
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float MinBurstTime = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float MaxBurstTime = 0.8f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float MinPauseTime = 0.45f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float MaxPauseTime = 1.0f;
};

UCLASS(BlueprintType)
class ASHLINEAI_API UALAIArchetypeData : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") FName ArchetypeId = TEXT("AI_Soldier_Basic");
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") FName SquadId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health") TObjectPtr<UALHealthConfig> HealthConfig;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon") TSoftObjectPtr<UALWeaponDataAsset> WeaponData;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float SightRange = 1800.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float LoseSightRange = 2250.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float PeripheralVision = 70.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float HearingRange = 1200.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float SuspiciousThreshold = 0.3f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float AlertThreshold = 0.7f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Detection") float CombatThreshold = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float ReactionTime = 0.45f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float Accuracy = 0.65f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") EALAIAccuracy AccuracyTier = EALAIAccuracy::Basic;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") FALBurstSettings BurstSettings;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float CombatRange = 1500.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat") float SearchDuration = 8.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float PatrolSpeed = 180.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float AlertSpeed = 260.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float CombatSpeed = 220.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cover") float CoverSearchRadius = 900.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cover") float CoverPreference = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Signals") float AlertPropagationRadius = 900.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Signals") float DamageAwarenessRadius = 1800.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Performance") float DecisionInterval = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Performance") float PerceptionInterval = 0.25f;
};
