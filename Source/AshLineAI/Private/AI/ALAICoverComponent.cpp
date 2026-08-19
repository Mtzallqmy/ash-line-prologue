#include "AI/ALAICoverComponent.h"
#include "World/ALCoverPoint.h"
#include "EngineUtils.h"
#include "Engine/World.h"

UALAICoverComponent::UALAICoverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

AALCoverPoint* UALAICoverComponent::FindAndReserveCover(float SearchRadius, bool bPreferCrouch)
{
    if (!GetWorld() || !GetOwner()) return nullptr;
    ReleaseCover();
    AALCoverPoint* BestPoint = nullptr;
    float BestScore = TNumericLimits<float>::Max();
    for (TActorIterator<AALCoverPoint> It(GetWorld()); It; ++It)
    {
        AALCoverPoint* Candidate = *It;
        if (!IsValid(Candidate) || Candidate->IsOccupied()) continue;
        const float Distance = FVector::DistSquared(Candidate->GetActorLocation(), GetOwner()->GetActorLocation());
        if (Distance > FMath::Square(SearchRadius)) continue;
        float Score = Distance;
        if (bPreferCrouch && Candidate->CoverType == EALCoverType::CrouchCover) Score *= 0.75f;
        if (Score < BestScore) { BestScore = Score; BestPoint = Candidate; }
    }
    if (BestPoint && BestPoint->TryReserve(GetOwner())) { CurrentCover = BestPoint; return BestPoint; }
    return nullptr;
}

void UALAICoverComponent::ReleaseCover()
{
    if (CurrentCover.IsValid()) CurrentCover->ReleaseReservation(GetOwner());
    CurrentCover.Reset();
}
