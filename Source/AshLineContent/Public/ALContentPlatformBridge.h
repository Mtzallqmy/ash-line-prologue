#pragma once
#include "CoreMinimal.h"
#include "ALContentPlatformBridge.generated.h"

UCLASS()
class ASHLINECONTENT_API UALContentPlatformBridge : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") static FString GetAppSpecificContentRoot();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") static bool OpenFilePickerForALPack();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") static bool IsSupportedPackageFile(const FString& FileReference);
};
