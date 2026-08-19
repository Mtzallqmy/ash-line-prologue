#pragma once
#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ALAssetManager.generated.h"

UCLASS()
class ASHLINECORE_API UALAssetManager : public UAssetManager
{
    GENERATED_BODY()
public:
    static UALAssetManager& Get();
    virtual void StartInitialLoading() override;
};
