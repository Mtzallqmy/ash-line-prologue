#include "ALWeaponBase.h"
#include "ALWeaponDataAsset.h"
#include "ALDamageSystemSubsystem.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"

AALWeaponBase::AALWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = Mesh;
}

int32 AALWeaponBase::GetMagazineSize() const { return Data ? Data->MagazineSize : 0; }

bool AALWeaponBase::Fire(const FVector& Origin, const FVector& Direction, AActor* InstigatorActor)
{
    if (!Data || CurrentAmmo <= 0 || !GetWorld() || GetWorld()->GetTimeSeconds() < NextFireTime) return false;
    CurrentAmmo--;
    NextFireTime = GetWorld()->GetTimeSeconds() + (1.0f / FMath::Max(Data->FireRate, 0.1f));
    FHitResult Hit;
    const FVector End = Origin + Direction.GetSafeNormal() * Data->Range;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ALWeaponFire), true, InstigatorActor);
    if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params) && Hit.GetActor())
    {
        FALDamageEvent Event;
        Event.Amount = Data->Damage;
        Event.Type = EALDamageType::Ballistic;
        Event.Origin = Origin;
        Event.InstigatorId = InstigatorActor ? InstigatorActor->GetFName() : NAME_None;
        GetWorld()->GetSubsystem<UALDamageSystemSubsystem>()->ApplyDamage(Hit.GetActor(), Event);
    }
    return true;
}

void AALWeaponBase::Reload() { if (Data) CurrentAmmo = Data->MagazineSize; }
