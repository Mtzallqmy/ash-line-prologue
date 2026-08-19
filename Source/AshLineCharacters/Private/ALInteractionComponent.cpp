#include "ALInteractionComponent.h"
#include "ALInteractable.h"
#include "GameFramework/Actor.h"

UALInteractionComponent::UALInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.10f;
}

void UALInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.TickInterval = FMath::Max(TraceInterval, 0.05f);
}

AActor* UALInteractionComponent::FindInteractable() const
{
    const AActor* Owner = GetOwner();
    if (!Owner || !GetWorld() || !bInteractionEnabled) return nullptr;

    FVector ViewLocation;
    FRotator ViewRotation;
    Owner->GetActorEyesViewPoint(ViewLocation, ViewRotation);

    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ALInteractionTrace), true, Owner);
    const FVector End = ViewLocation + ViewRotation.Vector() * InteractionRange;
    if (!GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_Visibility, QueryParams)) return nullptr;

    AActor* HitActor = Hit.GetActor();
    if (!IsValid(HitActor) || !HitActor->GetClass()->ImplementsInterface(UALInteractable::StaticClass())) return nullptr;
    return IALInteractable::Execute_CanInteract(HitActor, const_cast<AActor*>(Owner)) ? HitActor : nullptr;
}

void UALInteractionComponent::RefreshTarget()
{
    AActor* NewTarget = FindInteractable();
    AActor* OldTarget = CurrentTarget.Get();
    if (NewTarget != OldTarget)
    {
        CurrentTarget = NewTarget;
        OnInteractionAvailabilityChanged.Broadcast(IsValid(NewTarget), NewTarget);
    }
}

void UALInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bInteractionEnabled) return;
    TimeSinceTrace += DeltaTime;
    if (TimeSinceTrace >= FMath::Max(TraceInterval, 0.05f))
    {
        TimeSinceTrace = 0.0f;
        RefreshTarget();
    }
}

void UALInteractionComponent::TryInteract()
{
    if (!bInteractionEnabled) return;
    AActor* Target = CurrentTarget.Get();
    if (IsValid(Target) && Target->GetClass()->ImplementsInterface(UALInteractable::StaticClass()))
    {
        IALInteractable::Execute_Interact(Target, GetOwner());
    }
    else
    {
        RefreshTarget();
        Target = CurrentTarget.Get();
        if (IsValid(Target)) IALInteractable::Execute_Interact(Target, GetOwner());
    }
}

FText UALInteractionComponent::GetCurrentInteractionText() const
{
    AActor* Target = CurrentTarget.Get();
    return IsValid(Target) ? IALInteractable::Execute_GetInteractionText(Target) : FText::GetEmpty();
}

void UALInteractionComponent::SetInteractionEnabled(bool bEnabled)
{
    bInteractionEnabled = bEnabled;
    if (!bEnabled && CurrentTarget.IsValid())
    {
        CurrentTarget.Reset();
        OnInteractionAvailabilityChanged.Broadcast(false, nullptr);
    }
}
