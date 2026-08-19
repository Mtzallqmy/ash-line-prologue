#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALInteractable.h"
#include "ALDebugInteractableActor.generated.h"

UCLASS(NotBlueprintable, NotPlaceable)
class ASHLINECHARACTERS_API AALDebugInteractableActor : public AActor, public IALInteractable
{
    GENERATED_BODY()
public:
    AALDebugInteractableActor();
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionText_Implementation() const override;
    UPROPERTY(BlueprintReadOnly) int32 InteractionCount = 0;
};
