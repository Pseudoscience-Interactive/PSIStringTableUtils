// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "Internationalization/StringTableCore.h"
#include "Misc/AutomationTest.h"
#include "PSIStringTableUtilsHelper.h"
#include "PSIStringTableUtilsTestFixture.h"
#include "PSIStringTableUtilsTestObjects.h"

#if WITH_DEV_AUTOMATION_TESTS

// Regression test for a bug where 'bHasMetadata' was left uninitialized when an entry had no metadata,
// so the function could return garbage instead of reliably reporting 'false'.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperDoesEntryHaveMetadataTest, "PSI.StringTableUtils.Helper.DoesEntryHaveMetadata",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperDoesEntryHaveMetadataTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("HelperDoesEntryHaveMetadata"));

    // Several fresh keys are checked, rather than just one, since a stack-uninitialized bug like this one
    // does not necessarily reproduce with every call.
    //
    for (int32 Index = 0; Index < 5; ++Index)
    {
        const FString Key = FString::Printf(TEXT("NoMetadataKey%d"), Index);
        TableScope.SetSourceString(Key, TEXT("Value"));
        TestFalse(TEXT("An entry with no metadata reports 'false'"),
            FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetadata(TableScope.StringTable, Key));
    }

    TableScope.SetSourceString(TEXT("WithMetadataKey"), TEXT("Value"));
    TableScope.StringTable->SetMetaData(TEXT("WithMetadataKey"), FName(TEXT("MetaId")), TEXT("MetaValue"));
    TestTrue(TEXT("An entry with metadata reports 'true'"),
        FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetadata(TableScope.StringTable, TEXT("WithMetadataKey")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperMetadataAsStructRoundTripTest, "PSI.StringTableUtils.Helper.MetadataAsStructRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperMetadataAsStructRoundTripTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("HelperMetadataAsStruct"));
    TableScope.SetSourceString(TEXT("Key"), TEXT("Value"));

    FPSIStringTableUtilsTestMetaDataStruct InStruct;
    InStruct.Name = TEXT("Hello");
    InStruct.Count = 5;

    const bool bChanged = FPSIStringTableUtilsHelper::SetStringTableEntryMetadataFromStruct(TableScope.StringTable, TEXT("Key"), InStruct);
    TestTrue(TEXT("Setting metadata from a struct with non-default values reports a change"), bChanged);

    FPSIStringTableUtilsTestMetaDataStruct OutStruct;
    const int32 RetrievedValues = FPSIStringTableUtilsHelper::GetStringTableEntryMetadataAsStruct(TableScope.StringTable, TEXT("Key"), OutStruct);
    TestEqual(TEXT("Both struct properties are retrieved from the metadata"), RetrievedValues, 2);
    TestEqual(TEXT("The retrieved 'Name' matches what was set"), OutStruct.Name, InStruct.Name);
    TestEqual(TEXT("The retrieved 'Count' matches what was set"), OutStruct.Count, InStruct.Count);

    // Setting the struct back to its default values should remove the metadata again, since
    // 'SetStringTableEntryMetadataFromStruct' only keeps metadata for values that differ from the default.
    //
    const FPSIStringTableUtilsTestMetaDataStruct DefaultStruct;
    FPSIStringTableUtilsHelper::SetStringTableEntryMetadataFromStruct(TableScope.StringTable, TEXT("Key"), DefaultStruct);
    TestFalse(TEXT("No metadata remains after resetting the struct to its default values"),
        FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetadata(TableScope.StringTable, TEXT("Key")));

    FPSIStringTableUtilsTestMetaDataStruct StaleStruct;
    StaleStruct.Name = TEXT("StaleValue");
    StaleStruct.Count = 99;
    FPSIStringTableUtilsHelper::GetStringTableEntryMetadataAsStruct(TableScope.StringTable, TEXT("Key"), StaleStruct);
    TestTrue(TEXT("A property with no matching metadata is cleared back to its default"), StaleStruct.Name.IsEmpty());
    TestEqual(TEXT("A property with no matching metadata is cleared back to its default"), StaleStruct.Count, 0);

    return true;
}

// Before UE 5.8, the CSV functions use an own implementation, so this test covers both it and the engine's.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperCsvRoundTripTest, "PSI.StringTableUtils.Helper.CsvRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperCsvRoundTripTest::RunTest(const FString& Parameters)
{
    const FString TrickySourceString = TEXT("Say \"Hi\", then\nleave\tC:\\Path");
    const FString TrickyMetadata = TEXT("Line 1\nLine 2, \"quoted\"");

    const FPSIStringTableUtilsTestTableScope SourceTableScope(TEXT("HelperCsvSource"));
    SourceTableScope.SetSourceString(TEXT("Plain"), TEXT("Value"));
    SourceTableScope.SetSourceString(TEXT("Key, \"with\" quotes"), TrickySourceString);
    SourceTableScope.StringTable->SetMetaData(TEXT("Key, \"with\" quotes"), FName(TEXT("Speaker")), TrickyMetadata);

    const FString CSVString = FPSIStringTableUtilsHelper::ExportStringTableToCSVString(SourceTableScope.StringTable);
    TestTrue(TEXT("The CSV string starts with the 'Key' and 'SourceString' columns"), CSVString.StartsWith(TEXT("Key,SourceString")));

    const FPSIStringTableUtilsTestTableScope DestinationTableScope(TEXT("HelperCsvDestination"));
    DestinationTableScope.SetSourceString(TEXT("Stale"), TEXT("Removed by the import"));
    TestTrue(TEXT("The exported CSV string can be imported"),
        FPSIStringTableUtilsHelper::ImportStringTableFromCSVString(DestinationTableScope.StringTable, CSVString, TEXT("HelperCsvRoundTrip")));

    FString SourceString;
    TestFalse(TEXT("The import replaces the existing entries"), DestinationTableScope.StringTable->GetSourceString(TEXT("Stale"), SourceString));
    TestTrue(TEXT("The plain entry is imported"), DestinationTableScope.StringTable->GetSourceString(TEXT("Plain"), SourceString));
    TestEqual(TEXT("The plain entry keeps its source string"), SourceString, FString(TEXT("Value")));
    TestTrue(TEXT("The entry with special characters in its key is imported"),
        DestinationTableScope.StringTable->GetSourceString(TEXT("Key, \"with\" quotes"), SourceString));
    TestEqual(TEXT("Quotes, commas, line breaks, tabs and backslashes survive in the source string"), SourceString, TrickySourceString);
    TestEqual(TEXT("Quotes, commas and line breaks survive in the metadata"),
        DestinationTableScope.StringTable->GetMetaData(TEXT("Key, \"with\" quotes"), FName(TEXT("Speaker"))), TrickyMetadata);
    TestTrue(TEXT("An entry without metadata gets no metadata from the import"),
        DestinationTableScope.StringTable->GetMetaData(TEXT("Plain"), FName(TEXT("Speaker"))).IsEmpty());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_HelperCsvImportColumnsTest, "PSI.StringTableUtils.Helper.CsvImportColumns",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_HelperCsvImportColumnsTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("HelperCsvImportColumns"));

    // The columns may be in any order. A dev notes column is never imported as metadata.
    const FString CSVString = TEXT("Speaker,DevNotes,SourceString,Key\n\"Bob\",\"A note\",\"Hello\",\"Greeting\"\n");
    TestTrue(TEXT("A CSV string with reordered columns can be imported"),
        FPSIStringTableUtilsHelper::ImportStringTableFromCSVString(TableScope.StringTable, CSVString, TEXT("HelperCsvImportColumns")));

    FString SourceString;
    TableScope.StringTable->GetSourceString(TEXT("Greeting"), SourceString);
    TestEqual(TEXT("The source string is read from its column"), SourceString, FString(TEXT("Hello")));
    TestEqual(TEXT("The metadata is read from its column"),
        TableScope.StringTable->GetMetaData(TEXT("Greeting"), FName(TEXT("Speaker"))), FString(TEXT("Bob")));
    TestTrue(TEXT("The dev notes are not imported as metadata"),
        TableScope.StringTable->GetMetaData(TEXT("Greeting"), FName(TEXT("DevNotes"))).IsEmpty());

    AddExpectedMessage(TEXT("Failed to import"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 2);
    TestFalse(TEXT("A CSV string without entry rows is rejected"),
        FPSIStringTableUtilsHelper::ImportStringTableFromCSVString(TableScope.StringTable, TEXT("Key,SourceString\n"), TEXT("HeaderOnly")));
    TestFalse(TEXT("A CSV string without a 'SourceString' column is rejected"),
        FPSIStringTableUtilsHelper::ImportStringTableFromCSVString(TableScope.StringTable, TEXT("Key,Speaker\n\"A\",\"B\"\n"), TEXT("NoSourceString")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
