#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALContentManifest.h"
#include "ALContentDeliveryService.generated.h"

UINTERFACE(BlueprintType)
class ASHLINECONTENT_API UALContentDeliveryService : public UInterface { GENERATED_BODY() };
class ASHLINECONTENT_API IALContentDeliveryService
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool CheckManifest(FALContentManifest& OutManifest);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) TArray<FString> GetInstalledPackages();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool IsPackageInstalled(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool RequestPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) float GetDownloadProgress(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool MountPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool UnmountPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool VerifyPackage(const FString& PackageId);
};
