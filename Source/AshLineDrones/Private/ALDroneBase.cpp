#include "ALDroneBase.h"
#include "GameFramework/Pawn.h"
void UALDroneBatteryComponent::BeginPlay() { Super::BeginPlay(); PrimaryComponentTick.bCanEverTick = true; RemainingSeconds = BatterySeconds; }
void UALDroneBatteryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) { Super::TickComponent(DeltaTime, TickType, ThisTickFunction); RemainingSeconds = FMath::Max(0.0f, RemainingSeconds - DeltaTime); }
void UALDroneSensorComponent::MarkTarget(AActor* Target) { if (IsValid(Target)) MarkedTargets.AddUnique(Target); }
AALDroneBase::AALDroneBase()
{
    DroneMovementComponent = CreateDefaultSubobject<UALDroneMovementComponent>(TEXT("DroneMovementComponent"));
    DroneCameraComponent = CreateDefaultSubobject<UALDroneCameraComponent>(TEXT("DroneCameraComponent"));
    DroneBatteryComponent = CreateDefaultSubobject<UALDroneBatteryComponent>(TEXT("DroneBatteryComponent"));
    DroneSensorComponent = CreateDefaultSubobject<UALDroneSensorComponent>(TEXT("DroneSensorComponent"));
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
void AALDroneBase::SetOwnerPawn(APawn* NewOwner) { OwnerPawn = NewOwner; }
void AALDroneBase::ReturnToOwnerPawn() { if (OwnerPawn) SetActorLocation(OwnerPawn->GetActorLocation() + FVector(0, 0, 150)); }
