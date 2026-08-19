#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALCoverPoint.generated.h"

UENUM(BlueprintType)
enum class EALCoverType : uint8
{
    StandingCover,
    CrouchCover
};

UCLASS(Blueprintable)
class ASHLINEAI_API AALCoverPoint : public AActor
{
    GENERATED_BODY()
public:
    AALCoverPoint();
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Cover") EALCoverType CoverType = EALCoverType::StandingCover;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Cover") float ReservationRadius = 90.0f;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") bool TryReserve(AActor* Requester);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void ReleaseReservation(AActor* Requester);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") bool IsOccupied() const { return ReservedBy.IsValid(); }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") AActor* GetReservedBy() const { return ReservedBy.Get(); }
protected:
    TWeakObjectPtr<AActor> ReservedBy;
};
