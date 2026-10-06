// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "BlueprintAPI/PSIStringTableLibrary.h"
#include "Internationalization/StringTableCore.h"
#include "Misc/AutomationTest.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableId.h"
#include "PSIStringTableUtilsTestFixture.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryIsRegisteredTest, "PSI.StringTableUtils.Library.IsRegistered",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryIsRegisteredTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibraryIsRegistered"));
    TableScope.SetSourceString(TEXT("ExistingKey"), TEXT("Some Source String"));

    TestTrue(TEXT("'IsStringTableRegistered' is true for a registered table"),
        UPSIStringTableLibrary::IsStringTableRegistered(FPSIStringTableId(TableScope.TableId)));
    TestFalse(TEXT("'IsStringTableRegistered' is false for an unregistered table"),
        UPSIStringTableLibrary::IsStringTableRegistered(FPSIStringTableId(FName(TEXT("NotARegisteredTable")))));

    const FPSIStringTableEntryHandle ValidEntry(TableScope.TableId, FString(TEXT("ExistingKey")));
    const FPSIStringTableEntryHandle MissingEntry(TableScope.TableId, FString(TEXT("MissingKey")));
    TestTrue(TEXT("'IsStringTableEntryRegistered' is true for an existing entry"),
        UPSIStringTableLibrary::IsStringTableEntryRegistered(ValidEntry));
    TestFalse(TEXT("'IsStringTableEntryRegistered' is false for a missing entry"),
        UPSIStringTableLibrary::IsStringTableEntryRegistered(MissingEntry));
    TestTrue(TEXT("'IsValidStringTableEntryHandle' agrees with 'IsStringTableEntryRegistered'"),
        UPSIStringTableLibrary::IsValidStringTableEntryHandle(ValidEntry));
    TestFalse(TEXT("'IsValidStringTableEntryHandle' is false for a missing entry"),
        UPSIStringTableLibrary::IsValidStringTableEntryHandle(MissingEntry));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryNamespaceTest, "PSI.StringTableUtils.Library.Namespace",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryNamespaceTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibraryNamespace"));
    const FPSIStringTableId TableId(TableScope.TableId);

    UPSIStringTableLibrary::SetStringTableNamespace(TableId, TEXT("MyNamespace"));
    TestEqual(TEXT("'GetStringTableNamespace' returns the namespace that was set"),
        UPSIStringTableLibrary::GetStringTableNamespace(TableId), FString(TEXT("MyNamespace")));
#else
    AddInfo(TEXT("Skipped: 'SetStringTableNamespace' is only available 'WITH_EDITOR'."));
#endif

    return true;
}

// Also covers the regression for a bug where an empty key crashed the editor: the engine's
// 'FStringTable::SetSourceString' asserts that the key is non-empty, so passing an entry handle with a
// valid table but an empty key has to be rejected before reaching it.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibrarySourceStringTest, "PSI.StringTableUtils.Library.SourceString",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibrarySourceStringTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibrarySourceString"));

    const FPSIStringTableEntryHandle NewEntry(TableScope.TableId, FString(TEXT("NewKey")));
    const bool bSetResult = UPSIStringTableLibrary::SetStringTableSourceString(NewEntry, TEXT("Hello"), false);
    TestTrue(TEXT("'SetStringTableSourceString' succeeds for a valid table and a non-empty key"), bSetResult);
    TestEqual(TEXT("'GetStringTableSourceString' returns the string that was set"),
        UPSIStringTableLibrary::GetStringTableSourceString(NewEntry), FString(TEXT("Hello")));

    const FPSIStringTableEntryHandle EmptyKeyEntry(TableScope.TableId, FString());
    const bool bEmptyKeyResult = UPSIStringTableLibrary::SetStringTableSourceString(EmptyKeyEntry, TEXT("Hello"), false);
    TestFalse(TEXT("'SetStringTableSourceString' rejects an empty key instead of crashing"), bEmptyKeyResult);
#else
    AddInfo(TEXT("Skipped: 'SetStringTableSourceString' is only available 'WITH_EDITOR'."));
#endif

    return true;
}

// Also covers the regression for a bug where 'bPreserveData' crashed the editor for a key that did not exist yet.
// The missing entry was dereferenced to read its dev notes because the wrong pointer was null-checked.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibrarySourceStringPreserveDataTest, "PSI.StringTableUtils.Library.SourceStringPreserveData",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibrarySourceStringPreserveDataTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibrarySourceStringPreserveData"));

    const FPSIStringTableEntryHandle Entry(TableScope.TableId, FString(TEXT("NewKey")));
    const bool bNewResult = UPSIStringTableLibrary::SetStringTableSourceString(Entry, TEXT("Hello"), true);
    TestTrue(TEXT("'SetStringTableSourceString' with 'bPreserveData' creates a missing entry instead of crashing"), bNewResult);
    TestEqual(TEXT("The new entry has the source string that was set"),
        UPSIStringTableLibrary::GetStringTableSourceString(Entry), FString(TEXT("Hello")));

    UPSIStringTableLibrary::SetStringTableEntryMetaData(Entry, FName(TEXT("MetaId")), TEXT("MetaValue"));
    const bool bUpdateResult = UPSIStringTableLibrary::SetStringTableSourceString(Entry, TEXT("World"), true);
    TestTrue(TEXT("'SetStringTableSourceString' with 'bPreserveData' succeeds for an existing entry"), bUpdateResult);
    TestEqual(TEXT("The existing entry has the new source string"),
        UPSIStringTableLibrary::GetStringTableSourceString(Entry), FString(TEXT("World")));
    TestEqual(TEXT("The existing entry keeps its meta-data"),
        UPSIStringTableLibrary::GetStringTableEntryMetaData(Entry, FName(TEXT("MetaId"))), FString(TEXT("MetaValue")));
#else
    AddInfo(TEXT("Skipped: 'SetStringTableSourceString' is only available 'WITH_EDITOR'."));
#endif

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryKeysAndRemoveTest, "PSI.StringTableUtils.Library.KeysAndRemove",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryKeysAndRemoveTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibraryKeysAndRemove"));
    const FPSIStringTableId TableId(TableScope.TableId);

    TableScope.SetSourceString(TEXT("KeyA"), TEXT("A"));
    TableScope.SetSourceString(TEXT("KeyB"), TEXT("B"));

    const TArray<FString> Keys = UPSIStringTableLibrary::GetKeysFromStringTable(TableId);
    TestEqual(TEXT("'GetKeysFromStringTable' returns both entries"), Keys.Num(), 2);
    TestTrue(TEXT("'GetKeysFromStringTable' includes 'KeyA'"), Keys.Contains(TEXT("KeyA")));
    TestTrue(TEXT("'GetKeysFromStringTable' includes 'KeyB'"), Keys.Contains(TEXT("KeyB")));

    const FPSIStringTableEntryHandle EntryA(TableScope.TableId, FString(TEXT("KeyA")));
    const bool bRemoved = UPSIStringTableLibrary::RemoveStringTableEntry(EntryA);
    TestTrue(TEXT("'RemoveStringTableEntry' succeeds for an existing entry"), bRemoved);
    TestFalse(TEXT("A removed entry is no longer registered"), UPSIStringTableLibrary::IsStringTableEntryRegistered(EntryA));

    const bool bRemovedAgain = UPSIStringTableLibrary::RemoveStringTableEntry(EntryA);
    TestFalse(TEXT("'RemoveStringTableEntry' fails when the entry no longer exists"), bRemovedAgain);
#else
    AddInfo(TEXT("Skipped: 'RemoveStringTableEntry' is only available 'WITH_EDITOR'."));
#endif

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryMetaDataTest, "PSI.StringTableUtils.Library.MetaData",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryMetaDataTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibraryMetaData"));
    TableScope.SetSourceString(TEXT("Key"), TEXT("Value"));

    const FPSIStringTableEntryHandle Entry(TableScope.TableId, FString(TEXT("Key")));

    TestTrue(TEXT("'GetMetaDataIdsFromStringTableEntry' is empty before any meta-data is set"),
        UPSIStringTableLibrary::GetMetaDataIdsFromStringTableEntry(Entry).IsEmpty());

    const bool bSetMetaData = UPSIStringTableLibrary::SetStringTableEntryMetaData(Entry, FName(TEXT("MetaId")), TEXT("MetaValue"));
    TestTrue(TEXT("'SetStringTableEntryMetaData' succeeds for an existing entry"), bSetMetaData);
    TestEqual(TEXT("'GetStringTableEntryMetaData' returns the value that was set"),
        UPSIStringTableLibrary::GetStringTableEntryMetaData(Entry, FName(TEXT("MetaId"))), FString(TEXT("MetaValue")));

    const TArray<FName> MetaDataIds = UPSIStringTableLibrary::GetMetaDataIdsFromStringTableEntry(Entry);
    TestEqual(TEXT("'GetMetaDataIdsFromStringTableEntry' returns the one meta-data ID that was set"), MetaDataIds.Num(), 1);
    TestTrue(TEXT("'GetMetaDataIdsFromStringTableEntry' includes 'MetaId'"), MetaDataIds.Contains(FName(TEXT("MetaId"))));

    const bool bRemovedMetaData = UPSIStringTableLibrary::RemoveStringTableEntryMetaData(Entry, FName(TEXT("MetaId")));
    TestTrue(TEXT("'RemoveStringTableEntryMetaData' succeeds for an existing meta-data ID"), bRemovedMetaData);
    TestTrue(TEXT("The meta-data ID is gone after being removed"),
        UPSIStringTableLibrary::GetStringTableEntryMetaData(Entry, FName(TEXT("MetaId"))).IsEmpty());

    UPSIStringTableLibrary::SetStringTableEntryMetaData(Entry, FName(TEXT("AnotherMetaId")), TEXT("AnotherValue"));
    const bool bCleared = UPSIStringTableLibrary::ClearMetaDataFromStringTableEntry(Entry);
    TestTrue(TEXT("'ClearMetaDataFromStringTableEntry' succeeds when meta-data exists"), bCleared);
    TestTrue(TEXT("No meta-data IDs remain after clearing"),
        UPSIStringTableLibrary::GetMetaDataIdsFromStringTableEntry(Entry).IsEmpty());
#else
    AddInfo(TEXT("Skipped: meta-data mutation functions are only available 'WITH_EDITOR'."));
#endif

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryLiteralAndEqualityTest, "PSI.StringTableUtils.Library.LiteralAndEquality",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryLiteralAndEqualityTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableId IdA(FName(TEXT("TableA")));
    const FPSIStringTableId IdB(FName(TEXT("TableB")));
    TestTrue(TEXT("'MakeLiteralStringTableId' passes its value through unchanged"),
        UPSIStringTableLibrary::MakeLiteralStringTableId(IdA) == IdA);
    TestTrue(TEXT("'EqualEqual_StringTableId' agrees with 'operator=='"), UPSIStringTableLibrary::EqualEqual_StringTableId(IdA, IdA));
    TestTrue(TEXT("'NotEqual_StringTableId' agrees with 'operator!='"), UPSIStringTableLibrary::NotEqual_StringTableId(IdA, IdB));

    const FPSIStringTableEntryHandle EntryA(FName(TEXT("TableA")), FString(TEXT("KeyA")));
    const FPSIStringTableEntryHandle EntryB(FName(TEXT("TableB")), FString(TEXT("KeyB")));
    TestTrue(TEXT("'MakeLiteralStringTableEntryHandle' passes its value through unchanged"),
        UPSIStringTableLibrary::MakeLiteralStringTableEntryHandle(EntryA) == EntryA);
    TestTrue(TEXT("'EqualEqual_StringTableEntryHandle' agrees with 'operator=='"),
        UPSIStringTableLibrary::EqualEqual_StringTableEntryHandle(EntryA, EntryA));
    TestTrue(TEXT("'NotEqual_StringTableEntryHandle' agrees with 'operator!='"),
        UPSIStringTableLibrary::NotEqual_StringTableEntryHandle(EntryA, EntryB));

    return true;
}

// Regression test for a bug where 'GetStringTableEntryHandleFromText' always returned an empty handle.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryEntryHandleFromTextTest, "PSI.StringTableUtils.Library.EntryHandleFromText",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryEntryHandleFromTextTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("LibraryEntryHandleFromText"));
    TableScope.SetSourceString(TEXT("Key"), TEXT("Value"));

    const FText TableText = FText::FromStringTable(TableScope.TableId, TEXT("Key"));
    const FPSIStringTableEntryHandle Handle = UPSIStringTableLibrary::GetStringTableEntryHandleFromText(TableText);

    TestFalse(TEXT("The handle returned from a valid string-table 'FText' is not 'IsNone'"), Handle.IsNone());
    TestEqual(TEXT("The handle's table ID matches the source 'FText'"), Handle.TableId.Name, TableScope.TableId);
    TestEqual(TEXT("The handle's key matches the source 'FText'"), Handle.Key, FString(TEXT("Key")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_LibraryCsvRoundTripTest, "PSI.StringTableUtils.Library.CsvRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_LibraryCsvRoundTripTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope SourceTableScope(TEXT("LibraryCsvSource"));
    SourceTableScope.SetSourceString(TEXT("Key"), TEXT("Value"));
    const FPSIStringTableId SourceTableId(SourceTableScope.TableId);

    FString CSVString;
    const bool bExported = UPSIStringTableLibrary::ExportStringTableToCSVString(SourceTableId, CSVString);
    TestTrue(TEXT("'ExportStringTableToCSVString' succeeds for a registered table"), bExported);
    TestTrue(TEXT("The exported CSV string contains the exported key"), CSVString.Contains(TEXT("Key")));

    const FPSIStringTableUtilsTestTableScope DestinationTableScope(TEXT("LibraryCsvDestination"));
    const FPSIStringTableId DestinationTableId(DestinationTableScope.TableId);
    const bool bImported = UPSIStringTableLibrary::ImportStringTableFromCSVString(DestinationTableId, CSVString);
    TestTrue(TEXT("'ImportStringTableFromCSVString' succeeds for a registered table"), bImported);

    FString ImportedSourceString;
    DestinationTableScope.StringTable->GetSourceString(TEXT("Key"), ImportedSourceString);
    TestEqual(TEXT("The imported entry's source string matches the exported one"), ImportedSourceString, FString(TEXT("Value")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
