#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ALContentDeliveryService.h"
#include "ALLocalContentDeliveryService.generated.h"

UCLASS(BlueprintType)
class ASHLINECONTENT_API UALLocalContentDeliveryService : public UObject, public IALContentDeliveryService
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FALContentManifest Manifest;
    UPROPERTY(BlueprintReadOnly) TSet<FString> InstalledPackages;
    virtual bool CheckManifest_Implementation(FALContentManifest& OutManifest) override;
    virtual TArray<FString> GetInstalledPackages_Implementation() override;
    virtual bool IsPackageInstalled_Implementation(const FString& PackageId) override;
    virtual bool RequestPackage_Implementation(const FString& PackageId) override;
    virtual float GetDownloadProgress_Implementation(const FString& PackageId) override;
    virtual bool MountPackage_Implementation(const FString& PackageId) override;
    virtual bool UnmountPackage_Implementation(const FString& PackageId) override;
    virtual bool VerifyPackage_Implementation(const FString& PackageId) override;
};
