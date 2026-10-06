// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "Internationalization/StringTableCore.h"
#include "Misc/AutomationTest.h"
#include "PSIStringTableId.h"
#include "PSIStringTableUtilsTestFixture.h"
#include "UObject/PropertyPortFlags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_IdDefaultsTest, "PSI.StringTableUtils.Id.Defaults",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_IdDefaultsTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableId DefaultId;
    TestTrue(TEXT("A default-constructed ID 'IsNone'"), DefaultId.IsNone());

    const FPSIStringTableId NamedId(FName(TEXT("MyTable")));
    TestFalse(TEXT("An ID constructed from an 'FName' is not 'IsNone'"), NamedId.IsNone());
    TestEqual(TEXT("'ToString' matches the constructed name"), NamedId.ToString(), FString(TEXT("MyTable")));

    FPSIStringTableId ResetId = NamedId;
    ResetId.Reset();
    TestTrue(TEXT("'Reset' clears the ID back to 'IsNone'"), ResetId.IsNone());

    const FName ImplicitName = NamedId;
    TestEqual(TEXT("The implicit conversion to 'FName' matches 'Name'"), ImplicitName, NamedId.Name);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_IdEqualityTest, "PSI.StringTableUtils.Id.Equality",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_IdEqualityTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableId A(FName(TEXT("TableA")));
    const FPSIStringTableId SameAsA(FName(TEXT("TableA")));
    const FPSIStringTableId B(FName(TEXT("TableB")));

    TestTrue(TEXT("IDs with the same name are equal"), A == SameAsA);
    TestFalse(TEXT("IDs with a different name are not equal"), A == B);
    TestTrue(TEXT("IDs with a different name are unequal"), A != B);
    TestEqual(TEXT("Equal IDs hash the same"), GetTypeHash(A), GetTypeHash(SameAsA));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_IdImportExportTextTest, "PSI.StringTableUtils.Id.ImportExportText",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_IdImportExportTextTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableId OriginalId(FName(TEXT("MyTable")));

    FString ExportedText;
    OriginalId.ExportTextItem(ExportedText, OriginalId, nullptr, PPF_None, nullptr);
    TestEqual(TEXT("'ExportTextItem' without delimiters exports the raw name"), ExportedText, FString(TEXT("MyTable")));

    FPSIStringTableId ImportedId;
    const TCHAR* Buffer = *ExportedText;
    const bool bImported = ImportedId.ImportTextItem(Buffer, PPF_None, nullptr, nullptr);
    TestTrue(TEXT("'ImportTextItem' succeeds for a previously exported value"), bImported);
    TestTrue(TEXT("A round-tripped ID is equal to the original"), ImportedId == OriginalId);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPSIStringTableUtils_IdStringTableLookupTest, "PSI.StringTableUtils.Id.StringTableLookup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPSIStringTableUtils_IdStringTableLookupTest::RunTest(const FString& Parameters)
{
    const FPSIStringTableUtilsTestTableScope TableScope(TEXT("IdLookup"));

    const FPSIStringTableId RegisteredId(TableScope.TableId);
    TestTrue(TEXT("'GetStringTable' finds a table that is registered"), RegisteredId.GetStringTable().IsValid());

    const FPSIStringTableId UnregisteredId(FName(TEXT("NotARegisteredTable")));
    TestFalse(TEXT("'GetStringTable' does not find a table that is not registered"),
        UnregisteredId.GetStringTable().IsValid());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
