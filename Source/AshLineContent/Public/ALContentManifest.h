#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.generated.h"

USTRUCT(BlueprintType)
struct FALContentPackage
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Version = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int64 DownloadSizeBytes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int64 InstalledSizeBytes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Hash;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> Dependencies;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRequired = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Url;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AssetDeliveryIdentifier;
};

USTRUCT(BlueprintType)
struct FALContentManifest
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString GameVersion = TEXT("0.1.0");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ContentVersion = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FALContentPackage> Packages;
};
