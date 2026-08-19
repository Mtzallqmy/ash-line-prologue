#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ALGameInstance.generated.h"

UCLASS()
class ASHLINECORE_API UALGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") bool SaveProgress();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") bool LoadProgress();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") void SetGraphicsProfile(int32 ProfileIndex);
    UPROPERTY(BlueprintReadOnly) TObjectPtr<class UALSaveGame> CurrentSave;
};
