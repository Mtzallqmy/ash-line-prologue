#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ALSemanticVersion.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FALSemanticVersionComparisonTest, "ASHLINE.Content.SemanticVersion.Comparison", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FALSemanticVersionComparisonTest::RunTest(const FString& Parameters)
{
    FALSemanticVersion V010;
    FALSemanticVersion V090;
    FALSemanticVersion V100;
    TestTrue(TEXT("0.10.0 parses"), FALSemanticVersion::TryParse(TEXT("0.10.0"), V010));
    TestTrue(TEXT("0.9.0 parses"), FALSemanticVersion::TryParse(TEXT("0.9.0"), V090));
    TestTrue(TEXT("1.0.0 parses"), FALSemanticVersion::TryParse(TEXT("1.0.0"), V100));
    TestTrue(TEXT("0.10.0 is greater than 0.9.0 numerically"), V010 > V090);
    TestTrue(TEXT("1.0.0 is greater than 0.10.0"), V100 > V010);
    TestFalse(TEXT("Malformed version is rejected"), FALSemanticVersion::TryParse(TEXT("0.10"), V010));
    return true;
}

#endif
