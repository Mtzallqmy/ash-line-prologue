#include "World/ALPatrolRoute.h"

AALPatrolRoute::AALPatrolRoute()
{
    PrimaryActorTick.bCanEverTick = false;
}

FVector AALPatrolRoute::GetPoint(int32 Index) const
{
    return PatrolPoints.IsValidIndex(Index) ? GetActorTransform().TransformPosition(PatrolPoints[Index]) : GetActorLocation();
}

float AALPatrolRoute::GetWaitTime(int32 Index) const
{
    return WaitTimes.IsValidIndex(Index) ? FMath::Max(0.0f, WaitTimes[Index]) : 0.5f;
}

int32 AALPatrolRoute::GetNextIndex(int32 Index) const
{
    if (PatrolPoints.Num() == 0) return INDEX_NONE;
    const int32 Next = Index + 1;
    if (Next < PatrolPoints.Num()) return Next;
    return bLoop ? 0 : INDEX_NONE;
}
