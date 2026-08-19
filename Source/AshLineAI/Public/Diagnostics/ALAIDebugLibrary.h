#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ALAIDebugLibrary.generated.h"

UCLASS()
class ASHLINEAI_API UALAIDebugLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI|Development") static void SetAIFrozen(AActor* Actor, bool bFrozen);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI|Development") static void DrawAIDebug(AActor* Actor, float Duration = 0.3f);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI|Development") static FString GetAIStateLabel(AActor* Actor);
};
