#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ActorComponent.h"
#include "ALDroneBase.generated.h"

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEDRONES_API UALDroneMovementComponent : public UActorComponent { GENERATED_BODY() };
UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEDRONES_API UALDroneCameraComponent : public UActorComponent { GENERATED_BODY() };
UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEDRONES_API UALDroneBatteryComponent : public UActorComponent
{ GENERATED_BODY() public: UPROPERTY(EditAnywhere, BlueprintReadWrite) float BatterySeconds = 90.0f; UPROPERTY(BlueprintReadOnly) float RemainingSeconds = 90.0f; virtual void BeginPlay() override; virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; };
UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEDRONES_API UALDroneSensorComponent : public UActorComponent
{ GENERATED_BODY() public: UFUNCTION(BlueprintCallable) void MarkTarget(AActor* Target); UPROPERTY(BlueprintReadOnly) TArray<TObjectPtr<AActor>> MarkedTargets; };

UCLASS(Blueprintable)
class ASHLINEDRONES_API AALDroneBase : public APawn
{
    GENERATED_BODY()
public:
    AALDroneBase();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UALDroneMovementComponent> DroneMovementComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UALDroneCameraComponent> DroneCameraComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UALDroneBatteryComponent> DroneBatteryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UALDroneSensorComponent> DroneSensorComponent;
    UFUNCTION(BlueprintCallable) void ReturnToOwnerPawn();
    UFUNCTION(BlueprintCallable) void SetOwnerPawn(APawn* NewOwner);
protected:
    UPROPERTY() TObjectPtr<APawn> OwnerPawn;
};
