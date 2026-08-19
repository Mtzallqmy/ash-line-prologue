#pragma once

#include "CoreMinimal.h"

struct ASHLINECONTENT_API FALSemanticVersion
{
    int32 Major = 0;
    int32 Minor = 0;
    int32 Patch = 0;

    static bool TryParse(const FString& Text, FALSemanticVersion& OutVersion);
    FString ToString() const;
    bool operator==(const FALSemanticVersion& Other) const;
    bool operator!=(const FALSemanticVersion& Other) const;
    bool operator<(const FALSemanticVersion& Other) const;
    bool operator<=(const FALSemanticVersion& Other) const;
    bool operator>(const FALSemanticVersion& Other) const;
    bool operator>=(const FALSemanticVersion& Other) const;
};
