#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ALChunkManager.generated.h"

USTRUCT(BlueprintType)
struct FALChunkDefinition { GENERATED_BODY() UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 ChunkId = 0; UPROPERTY(EditAnywhere, BlueprintReadOnly) FName PackageId; UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> AssetRoots; };
UCLASS(BlueprintType)
class ASHLINECONTENT_API UALChunkManager : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FALChunkDefinition> Chunks;
    UFUNCTION(BlueprintCallable) int32 FindChunkForPackage(FName PackageId) const;
};
