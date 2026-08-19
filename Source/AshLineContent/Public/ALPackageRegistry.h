#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.h"
#include "ALPackageRegistry.generated.h"

USTRUCT(BlueprintType)
struct ASHLINECONTENT_API FALInstalledPackageRecord
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FString PackageId;
    UPROPERTY(BlueprintReadOnly) int32 Version = 0;
    UPROPERTY(BlueprintReadOnly) FString Path;
    UPROPERTY(BlueprintReadOnly) FString SHA256;
    UPROPERTY(BlueprintReadOnly) EALPackageState State = EALPackageState::Unknown;
    UPROPERTY(BlueprintReadOnly) bool bMounted = false;
    UPROPERTY(BlueprintReadOnly) int64 InstalledSizeBytes = 0;
    UPROPERTY(BlueprintReadOnly) FString InstallDateUtc;
    UPROPERTY(BlueprintReadOnly) TArray<FString> Dependencies;
};

UCLASS(BlueprintType)
class ASHLINECONTENT_API UALPackageRegistry : public UObject
{
    GENERATED_BODY()
public:
    bool Load(const FString& RegistryPath);
    bool Save(const FString& RegistryPath) const;
    bool RebuildFromDirectory(const FString& InstalledRoot, const FString& RegistryPath);
    void SetRecord(const FALInstalledPackageRecord& Record);
    bool RemoveRecord(const FString& PackageId);
    bool GetRecord(const FString& PackageId, FALInstalledPackageRecord& OutRecord) const;
    bool HasRecord(const FString& PackageId) const;
    TArray<FALInstalledPackageRecord> GetRecords() const;

protected:
    UPROPERTY() TArray<FALInstalledPackageRecord> Records;
};
