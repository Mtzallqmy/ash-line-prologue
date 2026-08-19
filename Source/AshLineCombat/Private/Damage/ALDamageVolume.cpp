#include "Damage/ALDamageVolume.h"
#include "ALDamageSystemSubsystem.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

AALDamageVolume::AALDamageVolume()
{
    PrimaryActorTick.bCanEverTick = false;
    Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
    RootComponent = Volume;
    Volume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AALDamageVolume::BeginPlay()
{
    Super::BeginPlay();
    Volume->OnComponentBeginOverlap.AddDynamic(this, &AALDamageVolume::OnVolumeBeginOverlap);
    Volume->OnComponentEndOverlap.AddDynamic(this, &AALDamageVolume::OnVolumeEndOverlap);
}

void AALDamageVolume::OnVolumeBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (!IsValid(OtherActor) || OtherActor == this) return;
    OverlappingActors.Add(OtherActor);
    if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(DamageTimer))
    {
        GetWorld()->GetTimerManager().SetTimer(DamageTimer, this, &AALDamageVolume::ApplyPeriodicDamage, FMath::Max(DamageInterval, 0.1f), true);
    }
}

void AALDamageVolume::OnVolumeEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
    OverlappingActors.Remove(OtherActor);
    if (OverlappingActors.Num() == 0 && GetWorld()) GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
}

void AALDamageVolume::ApplyPeriodicDamage()
{
    if (!GetWorld()) return;
    UALDamageSystemSubsystem* DamageSystem = GetWorld()->GetSubsystem<UALDamageSystemSubsystem>();
    if (!DamageSystem) return;
    for (auto It = OverlappingActors.CreateIterator(); It; ++It)
    {
        AActor* Target = It->Get();
        if (!IsValid(Target)) { It.RemoveCurrent(); continue; }
        FALDamageData DamageData;
        DamageData.BaseDamage = DamagePerApplication;
        DamageData.DamageType = DamageType;
        DamageData.DamageCauser = this;
        DamageSystem->ApplyDamage(Target, DamageData);
    }
}
