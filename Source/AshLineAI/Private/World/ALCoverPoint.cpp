#include "World/ALCoverPoint.h"

AALCoverPoint::AALCoverPoint()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AALCoverPoint::TryReserve(AActor* Requester)
{
    if (!IsValid(Requester)) return false;
    if (!ReservedBy.IsValid() || ReservedBy.Get() == Requester)
    {
        ReservedBy = Requester;
        return true;
    }
    return false;
}

void AALCoverPoint::ReleaseReservation(AActor* Requester)
{
    if (!ReservedBy.IsValid() || !Requester || ReservedBy.Get() == Requester) ReservedBy.Reset();
}
