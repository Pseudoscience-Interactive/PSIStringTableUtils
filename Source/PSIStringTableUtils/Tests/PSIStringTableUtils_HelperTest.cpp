// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "Internationalization/StringTableCore.h"
#include "Misc/AutomationTest.h"
#include "PSIStringTableUtilsHelper.h"
#include "PSIStringTableUtilsTestFixture.h"
#include "PSIStringTableUtilsTestObjects.h"

#if WITH_DEV_AUTOMATION_TESTS

// Regression test for a bug where 'bHasMetaData' was left uninitialized when an entry had no meta-data,
// so the function could return garbage instead of reliably reporting 'false'.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperDoesEntryHaveMetaDataTest, "PSIStringTableUtils.Helper.DoesEntryHaveMetaData",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperDoesEntryHaveMetaDataTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("HelperDoesEntryHaveMetaData"));

    // Several fresh keys are checked, rather than just one, since a stack-uninitialized bug like this one
    // does not necessarily reproduce with every call.
    //
    for (int32 Index = 0; Index < 5; ++Index)
    {
        const FString Key = FString::Printf(TEXT("NoMetaDataKey%d"), Index);
        TableScope.SetSourceString(Key, TEXT("Value"));
        TestFalse(TEXT("An entry with no meta-data reports 'false'"),
            FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetaData(TableScope.StringTable, Key));
    }

    TableScope.SetSourceString(TEXT("WithMetaDataKey"), TEXT("Value"));
    TableScope.StringTable->SetMetaData(TEXT("WithMetaDataKey"), FName(TEXT("MetaId")), TEXT("MetaValue"));
    TestTrue(TEXT("An entry with meta-data reports 'true'"),
        FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetaData(TableScope.StringTable, TEXT("WithMetaDataKey")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperMetaDataAsStructRoundTripTest, "PSIStringTableUtils.Helper.MetaDataAsStructRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperMetaDataAsStructRoundTripTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("HelperMetaDataAsStruct"));
    TableScope.SetSourceString(TEXT("Key"), TEXT("Value"));

    FPSIStringTableUtilsTestMetaDataStruct InStruct;
    InStruct.Name = TEXT("Hello");
    InStruct.Count = 5;

    const bool bChanged = FPSIStringTableUtilsHelper::SetStringTableEntryMetaDataFromStruct(TableScope.StringTable, TEXT("Key"), InStruct);
    TestTrue(TEXT("Setting meta-data from a struct with non-default values reports a change"), bChanged);

    FPSIStringTableUtilsTestMetaDataStruct OutStruct;
    const int32 RetrievedValues = FPSIStringTableUtilsHelper::GetStringTableEntryMetaDataAsStruct(TableScope.StringTable, TEXT("Key"), OutStruct);
    TestEqual(TEXT("Both struct properties are retrieved from the meta-data"), RetrievedValues, 2);
    TestEqual(TEXT("The retrieved 'Name' matches what was set"), OutStruct.Name, InStruct.Name);
    TestEqual(TEXT("The retrieved 'Count' matches what was set"), OutStruct.Count, InStruct.Count);

    // Setting the struct back to its default values should remove the meta-data again, since
    // 'SetStringTableEntryMetaDataFromStruct' only keeps meta-data for values that differ from the default.
    //
    const FPSIStringTableUtilsTestMetaDataStruct DefaultStruct;
    FPSIStringTableUtilsHelper::SetStringTableEntryMetaDataFromStruct(TableScope.StringTable, TEXT("Key"), DefaultStruct);
    TestFalse(TEXT("No meta-data remains after resetting the struct to its default values"),
        FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetaData(TableScope.StringTable, TEXT("Key")));

    FPSIStringTableUtilsTestMetaDataStruct StaleStruct;
    StaleStruct.Name = TEXT("StaleValue");
    StaleStruct.Count = 99;
    FPSIStringTableUtilsHelper::GetStringTableEntryMetaDataAsStruct(TableScope.StringTable, TEXT("Key"), StaleStruct);
    TestTrue(TEXT("A property with no matching meta-data is cleared back to its default"), StaleStruct.Name.IsEmpty());
    TestEqual(TEXT("A property with no matching meta-data is cleared back to its default"), StaleStruct.Count, 0);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
