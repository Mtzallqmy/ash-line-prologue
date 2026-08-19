#include "AI/ALAISignificanceComponent.h"

UALAISignificanceComponent::UALAISignificanceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALAISignificanceComponent::UpdateSignificance(float DistanceToPlayer)
{
    if (DistanceToPlayer <= NearDistance) Significance = EALAISignificance::Near;
    else if (DistanceToPlayer <= MediumDistance) Significance = EALAISignificance::Medium;
    else if (DistanceToPlayer <= FarDistance) Significance = EALAISignificance::Far;
    else Significance = EALAISignificance::Inactive;
}

float UALAISignificanceComponent::GetUpdateMultiplier() const
{
    switch (Significance)
    {
        case EALAISignificance::Near: return 1.0f;
        case EALAISignificance::Medium: return 1.5f;
        case EALAISignificance::Far: return 3.0f;
        default: return 5.0f;
    }
}
