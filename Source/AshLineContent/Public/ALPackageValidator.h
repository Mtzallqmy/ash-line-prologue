#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.h"
#include "ALPackageValidator.generated.h"

UCLASS()
class ASHLINECONTENT_API UALPackageValidator : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool ValidateManifest(const FALContentManifest& Manifest, const FString& GameVersion, bool bRequireSignature, EALContentError& OutError, FString& OutMessage) const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool ValidatePackage(const FALContentPackage& Package, const FString& GameVersion, bool bRequireSignature, EALContentError& OutError, FString& OutMessage) const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool ValidatePath(const FString& RelativePath, EALContentError& OutError, FString& OutMessage) const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool VerifySHA256(const FString& ExpectedHash, const FString& ActualHash, EALContentError& OutError, FString& OutMessage) const;
    bool ComputeDevelopmentDirectoryHash(const FString& DirectoryPath, FString& OutHash, FString& OutMessage) const;
    bool ResolveDependencies(const FString& PackageId, const FALContentManifest& Manifest, TArray<FString>& OutDependencies, EALContentError& OutError, FString& OutMessage) const;

    UPROPERTY(EditDefaultsOnly, Category="Limits") int64 MaxPackageSizeBytes = 5368709120LL;
    UPROPERTY(EditDefaultsOnly, Category="Limits") int32 MaxManifestBytes = 1048576;
    UPROPERTY(EditDefaultsOnly, Category="Limits") int32 MaxFileCount = 100000;
};
