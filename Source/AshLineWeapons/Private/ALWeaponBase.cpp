#include "ALWeaponBase.h"
#include "Data/ALWeaponData.h"
#include "Components/ALRecoilComponent.h"
#include "ALDamageSystemSubsystem.h"
#include "Damage/ALDamageData.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogAshLineWeapons, Log, All);

AALWeaponBase::AALWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(WeaponMesh);
    RecoilComponent = CreateDefaultSubobject<UALRecoilComponent>(TEXT("RecoilComponent"));
}

void AALWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    if (Data)
    {
        AmmoInMagazine = Data->MagazineSize;
        ReserveAmmo = Data->ReserveAmmo;
        if (WeaponMesh) if (USkeletalMesh* LoadedMesh = Data->WeaponMesh.LoadSynchronous()) WeaponMesh->SetSkeletalMesh(LoadedMesh);
        if (MuzzlePoint) MuzzlePoint->SetRelativeLocation(FVector::ZeroVector);
    }
    BroadcastAmmoChanged();
}

void AALWeaponBase::InitializeWeapon(APawn* NewOwner)
{
    SetOwningActor(NewOwner);
    if (Data && AmmoInMagazine <= 0) { AmmoInMagazine = Data->MagazineSize; ReserveAmmo = Data->ReserveAmmo; BroadcastAmmoChanged(); }
}

void AALWeaponBase::SetOwningActor(APawn* NewOwner)
{
    OwningPawn = NewOwner;
    SetOwner(NewOwner);
    SetInstigator(NewOwner);
}

void AALWeaponBase::EquipWeapon(APawn* NewOwner)
{
    InitializeWeapon(NewOwner);
    if (USkeletalMeshComponent* OwnerMesh = NewOwner ? NewOwner->FindComponentByClass<USkeletalMeshComponent>() : nullptr)
    {
        AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
    }
    WeaponState = EALWeaponState::Idle;
}

void AALWeaponBase::UnequipWeapon()
{
    StopFire();
    CancelReload();
    StopAim();
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    OwningPawn = nullptr;
    SetOwner(nullptr);
    WeaponState = EALWeaponState::Unequipping;
}

void AALWeaponBase::StartFire()
{
    if (!bWeaponInputEnabled || !Data || !OwningPawn || WeaponState == EALWeaponState::Equipping || WeaponState == EALWeaponState::Unequipping) return;
    if (WeaponState == EALWeaponState::Reloading) CancelReload();
    const bool bFired = TryFireSingle();
    if (!bFired && AmmoInMagazine <= 0) return;
    if (Data->FireMode == EALFireMode::FullAuto)
    {
        WeaponState = EALWeaponState::Firing;
        if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &AALWeaponBase::HandleAutomaticFire, Data->GetSecondsPerShot(), true);
    }
}

void AALWeaponBase::StopFire()
{
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(FireTimer);
    if (WeaponState == EALWeaponState::Firing) WeaponState = EALWeaponState::Idle;
}

void AALWeaponBase::HandleAutomaticFire()
{
    if (!bWeaponInputEnabled || WeaponState != EALWeaponState::Firing || !TryFireSingle()) StopFire();
}

bool AALWeaponBase::TryFireSingle()
{
    if (!Data || !OwningPawn || !bWeaponInputEnabled) return false;
    if (GetWorld() && GetWorld()->GetTimeSeconds() < NextFireTime) return false;
    if (AmmoInMagazine <= 0)
    {
        OnDryFire.Broadcast(GetWeaponId());
        UE_LOG(LogAshLineWeapons, Verbose, TEXT("Dry fire: %s"), *GetWeaponId().ToString());
        return false;
    }

    --AmmoInMagazine;
    NextFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() + Data->GetSecondsPerShot() : 0.0f;
    FireHitscan(AimOrigin.IsNearlyZero() ? OwningPawn->GetPawnViewLocation() : AimOrigin, AimDirection.IsNearlyZero() ? OwningPawn->GetViewRotation().Vector() : AimDirection);
    if (RecoilComponent) RecoilComponent->ApplyRecoil(Data->RecoilSettings, false);
    if (USoundBase* FireSound = Data->FireSound.LoadSynchronous()) UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    if (MuzzlePoint) if (UNiagaraSystem* MuzzleFX = Data->MuzzleFX.LoadSynchronous()) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFX, MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation());
    BroadcastAmmoChanged();
    OnWeaponFired.Broadcast();
    return true;
}

void AALWeaponBase::FireHitscan(const FVector& Origin, const FVector& Direction)
{
    const FVector ShotDirection = ApplySpread(Direction);
    const FVector End = Origin + ShotDirection * Data->Range;
    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ALWeaponHitscan), true, OwningPawn);
    if (!GetWorld() || !GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, QueryParams) || !Hit.GetActor()) return;

    FALDamageData DamageData;
    DamageData.BaseDamage = Data->Damage * Data->HitZones.GetMultiplier(Hit.BoneName);
    DamageData.DamageType = EALDamageType::Bullet;
    DamageData.Instigator = OwningPawn->GetController();
    DamageData.DamageCauser = this;
    DamageData.HitLocation = Hit.ImpactPoint;
    DamageData.HitNormal = Hit.ImpactNormal;
    DamageData.BoneName = Hit.BoneName;
    DamageData.bHasHitResult = Hit.bBlockingHit;
    GetWorld()->GetSubsystem<UALDamageSystemSubsystem>()->ApplyDamage(Hit.GetActor(), DamageData);
}

FVector AALWeaponBase::ApplySpread(const FVector& Direction) const
{
    const float SpreadDegrees = GetCurrentSpread();
    return FMath::VRandCone(Direction.GetSafeNormal(), FMath::DegreesToRadians(FMath::Max(SpreadDegrees, 0.0f)));
}

float AALWeaponBase::GetCurrentSpread() const
{
    if (!Data) return 0.0f;
    float Spread = bIsAiming ? Data->ADSSpread : Data->HipFireSpread;
    if (OwningPawn && OwningPawn->GetVelocity().SizeSquared2D() > 25.0f) Spread *= Data->MovementSpreadMultiplier;
    if (const ACharacter* Character = Cast<ACharacter>(OwningPawn)) if (Character->bIsCrouched) Spread *= Data->CrouchSpreadMultiplier;
    return Spread;
}

void AALWeaponBase::StartReload()
{
    if (!bWeaponInputEnabled || !Data || WeaponState == EALWeaponState::Reloading || AmmoInMagazine >= Data->MagazineSize || ReserveAmmo <= 0) return;
    StopFire();
    WeaponState = EALWeaponState::Reloading;
    OnReloadStarted.Broadcast();
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AALWeaponBase::CompleteReload, FMath::Max(Data->ReloadTime, 0.05f), false);
}

void AALWeaponBase::CompleteReload()
{
    if (!Data || WeaponState != EALWeaponState::Reloading) return;
    const int32 Needed = Data->MagazineSize - AmmoInMagazine;
    const int32 Transfer = FMath::Min(Needed, ReserveAmmo);
    AmmoInMagazine += Transfer;
    ReserveAmmo -= Transfer;
    WeaponState = EALWeaponState::Idle;
    BroadcastAmmoChanged();
    OnReloadCompleted.Broadcast();
}

void AALWeaponBase::CancelReload()
{
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
    if (WeaponState == EALWeaponState::Reloading)
    {
        WeaponState = EALWeaponState::Idle;
        OnReloadCancelled.Broadcast();
    }
}

void AALWeaponBase::StartAim()
{
    if (!bWeaponInputEnabled || !Data || WeaponState == EALWeaponState::Reloading) return;
    bIsAiming = true;
    OnAimChanged.Broadcast(true);
}

void AALWeaponBase::StopAim()
{
    if (!bIsAiming) return;
    bIsAiming = false;
    OnAimChanged.Broadcast(false);
}

void AALWeaponBase::SetWeaponInputEnabled(bool bEnabled)
{
    bWeaponInputEnabled = bEnabled;
    if (!bEnabled) { StopFire(); CancelReload(); StopAim(); }
}

void AALWeaponBase::AddReserveAmmo(int32 Amount)
{
    if (Amount <= 0) return;
    ReserveAmmo = FMath::Max(0, ReserveAmmo + Amount);
    BroadcastAmmoChanged();
}

void AALWeaponBase::SetAimDirection(const FVector& Origin, const FVector& Direction)
{
    AimOrigin = Origin;
    AimDirection = Direction.GetSafeNormal();
}

bool AALWeaponBase::FireAtTarget(AActor* Target, const FVector& Origin, const FVector& Direction)
{
    if (!IsValid(Target)) return false;
    SetAimDirection(Origin, Direction);
    StartFire();
    return true;
}

void AALWeaponBase::BroadcastAmmoChanged()
{
    OnAmmoChanged.Broadcast(AmmoInMagazine, ReserveAmmo);
}

int32 AALWeaponBase::GetMagazineSize() const { return Data ? Data->MagazineSize : 0; }
FName AALWeaponBase::GetWeaponId() const { return Data ? Data->WeaponId : NAME_None; }

float AALWeaponBase::GetDefaultFOV() const { return Data ? Data->DefaultFOV : 90.0f; }
float AALWeaponBase::GetADSFOV() const { return Data ? Data->ADSFOV : 70.0f; }
float AALWeaponBase::GetAimTransitionTime() const { return Data ? Data->AimTransitionTime : 0.15f; }
