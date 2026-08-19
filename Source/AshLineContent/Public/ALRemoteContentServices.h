#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ALContentDeliveryService.h"
#include "ALRemoteContentServices.generated.h"

UCLASS(BlueprintType)
class ASHLINECONTENT_API UALHttpContentService : public UObject, public IALContentDeliveryService
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CDN") FString ContentBaseURL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CDN") FString ManifestURL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CDN") FString Environment = TEXT("Development");
    virtual bool FetchManifest_Implementation(FALContentManifest& OutManifest) override;
    virtual bool CheckManifest_Implementation(FALContentManifest& OutManifest) override;
    virtual TArray<FString> GetInstalledPackages_Implementation() override;
    virtual bool IsPackageInstalled_Implementation(const FString& PackageId) override;
    virtual bool RequestPackage_Implementation(const FString& PackageId) override;
    virtual bool ImportPackage_Implementation(const FString& FileReference) override;
    virtual bool PauseDownload_Implementation(const FString& PackageId) override;
    virtual bool ResumeDownload_Implementation(const FString& PackageId) override;
    virtual bool CancelDownload_Implementation(const FString& PackageId) override;
    virtual float GetDownloadProgress_Implementation(const FString& PackageId) override;
    virtual bool MountPackage_Implementation(const FString& PackageId) override;
    virtual bool UnmountPackage_Implementation(const FString& PackageId) override;
    virtual bool VerifyPackage_Implementation(const FString& PackageId) override;
    virtual FString GetPackageFilePath_Implementation(const FString& PackageId) override;
};

UCLASS(BlueprintType)
class ASHLINECONTENT_API UALPlayAssetDeliveryService : public UObject, public IALContentDeliveryService
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Play Asset Delivery") FString Environment = TEXT("Development");
    virtual bool FetchManifest_Implementation(FALContentManifest& OutManifest) override;
    virtual bool CheckManifest_Implementation(FALContentManifest& OutManifest) override;
    virtual TArray<FString> GetInstalledPackages_Implementation() override;
    virtual bool IsPackageInstalled_Implementation(const FString& PackageId) override;
    virtual bool RequestPackage_Implementation(const FString& PackageId) override;
    virtual bool ImportPackage_Implementation(const FString& FileReference) override;
    virtual bool PauseDownload_Implementation(const FString& PackageId) override;
    virtual bool ResumeDownload_Implementation(const FString& PackageId) override;
    virtual bool CancelDownload_Implementation(const FString& PackageId) override;
    virtual float GetDownloadProgress_Implementation(const FString& PackageId) override;
    virtual bool MountPackage_Implementation(const FString& PackageId) override;
    virtual bool UnmountPackage_Implementation(const FString& PackageId) override;
    virtual bool VerifyPackage_Implementation(const FString& PackageId) override;
    virtual FString GetPackageFilePath_Implementation(const FString& PackageId) override;
};
