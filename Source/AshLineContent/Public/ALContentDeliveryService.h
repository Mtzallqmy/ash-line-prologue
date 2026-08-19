#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALContentManifest.h"
#include "ALContentDeliveryService.generated.h"

UINTERFACE(BlueprintType)
class ASHLINECONTENT_API UALContentDeliveryService : public UInterface
{
    GENERATED_BODY()
};

class ASHLINECONTENT_API IALContentDeliveryService
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool FetchManifest(FALContentManifest& OutManifest);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool CheckManifest(FALContentManifest& OutManifest);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") TArray<FString> GetInstalledPackages();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool IsPackageInstalled(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool RequestPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool ImportPackage(const FString& FileReference);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool PauseDownload(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool ResumeDownload(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool CancelDownload(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") float GetDownloadProgress(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool MountPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool UnmountPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") bool VerifyPackage(const FString& PackageId);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Content") FString GetPackageFilePath(const FString& PackageId);
};
