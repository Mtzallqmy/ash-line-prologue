#include "ALNoiseSystemSubsystem.h"
#include "Engine/World.h"

void UALNoiseSystemSubsystem::ReportNoise(AActor* Source, const FVector& Location, float Loudness, FName Tag)
{
    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    for (int32 Index = Events.Num() - 1; Index >= 0; --Index)
    {
        if (Now - Events[Index].TimeSeconds > 2.0f || !Events[Index].Source.IsValid()) Events.RemoveAtSwap(Index);
    }
    FALNoiseEvent Event;
    Event.Source = Source;
    Event.Location = Location;
    Event.Loudness = FMath::Max(0.0f, Loudness);
    Event.TimeSeconds = Now;
    Event.Tag = Tag;
    Events.Add(Event);
}

void UALNoiseSystemSubsystem::GetRecentEvents(float SinceTime, TArray<FALNoiseEvent>& OutEvents) const
{
    OutEvents.Reset();
    for (const FALNoiseEvent& Event : Events)
    {
        if (Event.TimeSeconds >= SinceTime && Event.Source.IsValid()) OutEvents.Add(Event);
    }
}
