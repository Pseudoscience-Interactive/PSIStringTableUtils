// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "Internationalization/StringTableCore.h"
#include "Misc/AutomationTest.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableUtilsTestFixture.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_EntryHandleDefaultsTest, "PSIStringTableUtils.EntryHandle.Defaults",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_EntryHandleDefaultsTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableEntryHandle DefaultHandle;
    TestTrue(TEXT("A default-constructed handle 'IsNone'"), DefaultHandle.IsNone());
    TestTrue(TEXT("A default-constructed handle has an empty 'ToString'"), DefaultHandle.ToString().IsEmpty());

    const FPSIStringTableEntryHandle NameKeyHandle(FName(TEXT("MyTable")), FString(TEXT("MyKey")));
    TestFalse(TEXT("A handle with a table ID and key is not 'IsNone'"), NameKeyHandle.IsNone());
    TestEqual(TEXT("'GetTextKey' matches the constructed key"), NameKeyHandle.GetTextKey(), FTextKey(TEXT("MyKey")));
    TestEqual(TEXT("'ToString' combines the table ID and key"), NameKeyHandle.ToString(), FString(TEXT("MyTable:MyKey")));

    FPSIStringTableEntryHandle ResetHandle = NameKeyHandle;
    ResetHandle.Reset();
    TestTrue(TEXT("'Reset' clears the handle back to 'IsNone'"), ResetHandle.IsNone());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_EntryHandleEqualityTest, "PSIStringTableUtils.EntryHandle.Equality",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_EntryHandleEqualityTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableEntryHandle A(FName(TEXT("TableA")), FString(TEXT("KeyA")));
    const FPSIStringTableEntryHandle SameAsA(FName(TEXT("TableA")), FString(TEXT("KeyA")));
    const FPSIStringTableEntryHandle DifferentKey(FName(TEXT("TableA")), FString(TEXT("KeyB")));
    const FPSIStringTableEntryHandle DifferentTable(FName(TEXT("TableB")), FString(TEXT("KeyA")));

    TestTrue(TEXT("Handles with the same table ID and key are equal"), A == SameAsA);
    TestFalse(TEXT("Handles with a different key are not equal"), A == DifferentKey);
    TestFalse(TEXT("Handles with a different table ID are not equal"), A == DifferentTable);
    TestTrue(TEXT("Handles with a different key are unequal"), A != DifferentKey);
    TestEqual(TEXT("Equal handles hash the same"), GetTypeHash(A), GetTypeHash(SameAsA));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_EntryHandleStringTableLookupTest, "PSIStringTableUtils.EntryHandle.StringTableLookup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_EntryHandleStringTableLookupTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("EntryHandleLookup"));
    TableScope.SetSourceString(TEXT("ExistingKey"), TEXT("Some Source String"));

    const FPSIStringTableEntryHandle ValidHandle(TableScope.TableId, FString(TEXT("ExistingKey")));
    TestTrue(TEXT("'GetStringTable' finds a table that is registered"), ValidHandle.GetStringTable().IsValid());
    TestTrue(TEXT("'GetStringTableEntry' finds an entry that exists"), ValidHandle.GetStringTableEntry().IsValid());

    const FPSIStringTableEntryHandle MissingKeyHandle(TableScope.TableId, FString(TEXT("MissingKey")));
    TestFalse(TEXT("'GetStringTableEntry' does not find an entry that does not exist"),
        MissingKeyHandle.GetStringTableEntry().IsValid());

    const FPSIStringTableEntryHandle UnregisteredTableHandle(FName(TEXT("NotARegisteredTable")), FString(TEXT("ExistingKey")));
    TestFalse(TEXT("'GetStringTable' does not find a table that is not registered"),
        UnregisteredTableHandle.GetStringTable().IsValid());

    const FPSIStringTableEntryHandle EmptyKeyHandle(TableScope.TableId, FString());
    TestFalse(TEXT("'GetStringTableEntry' returns nothing for an empty key, even on a valid table"),
        EmptyKeyHandle.GetStringTableEntry().IsValid());

    return true;
}

// Regression test for a bug where 'GetStringTableEntryHandleFromText'/the 'FText' constructor discarded the
// values it read into a throwaway local instead of writing them onto '*this', so the resulting handle was
// always empty.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_EntryHandleFromTextTest, "PSIStringTableUtils.EntryHandle.FromText",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_EntryHandleFromTextTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("EntryHandleFromText"));
    TableScope.SetSourceString(TEXT("MyKey"), TEXT("Some Source String"));

    const FText TableText = FText::FromStringTable(TableScope.TableId, TEXT("MyKey"));
    const FPSIStringTableEntryHandle HandleFromText(TableText);

    TestEqual(TEXT("The handle's table ID matches the source 'FText'"), HandleFromText.TableId.Name, TableScope.TableId);
    TestEqual(TEXT("The handle's key matches the source 'FText'"), HandleFromText.Key, FString(TEXT("MyKey")));
    TestFalse(TEXT("A handle built from a valid string-table 'FText' is not 'IsNone'"), HandleFromText.IsNone());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
