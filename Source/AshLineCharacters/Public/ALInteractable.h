#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALInteractable.generated.h"

UINTERFACE(BlueprintType)
class ASHLINECHARACTERS_API UALInteractable : public UInterface
{
    GENERATED_BODY()
};

class ASHLINECHARACTERS_API IALInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Interaction")
    bool CanInteract(AActor* Interactor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Interaction")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Interaction")
    FText GetInteractionText() const;
};
