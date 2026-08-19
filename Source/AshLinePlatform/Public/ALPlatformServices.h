#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALPlatformServices.generated.h"

UINTERFACE(BlueprintType)
class ASHLINEPLATFORM_API UALPlatformServices : public UInterface { GENERATED_BODY() };
class ASHLINEPLATFORM_API IALPlatformServices
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool RequestStoragePermission();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool OpenNotificationChannel(const FString& ChannelId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FString GetPlatformContentRoot();
};
