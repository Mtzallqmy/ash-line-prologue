#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALDamageTypes.h"
#include "ALDamageVolume.generated.h"

UCLASS(Blueprintable)
class ASHLINECOMBAT_API AALDamageVolume : public AActor
{
    GENERATED_BODY()
public:
    AALDamageVolume();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage Volume") TObjectPtr<class UBoxComponent> Volume;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Volume") float DamagePerApplication = 25.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Volume") float DamageInterval = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Volume") EALDamageType DamageType = EALDamageType::Environmental;

protected:
    UFUNCTION() void OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION() void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    void ApplyPeriodicDamage();

    TSet<TWeakObjectPtr<AActor>> OverlappingActors;
    FTimerHandle DamageTimer;
};
