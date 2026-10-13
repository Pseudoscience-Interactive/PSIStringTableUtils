// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableUtilsHelper.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Misc/Paths.h"
#include "PSIEngineVersionComparison.h"
#include "PSIStringTableUtilsLog.h"
#include "PSIStringTableUtilsModule.h"
#include "Serialization/Csv/CsvParser.h"
#include "UObject/Class.h"
#include "UObject/FieldIterator.h"

int32 FPSIStringTableUtilsHelper::GetStringTableEntryMetaDataAsStruct(const FStringTableConstPtr& StringTable,
    const FTextKey& TextKey, const UScriptStruct* Struct, void* StructValuePtr)
{
    int32 ChangedProperties = 0;
    if (Struct == nullptr || StructValuePtr == nullptr || TextKey.IsEmpty() || !StringTable.IsValid())
    {
        return ChangedProperties;
    }
    
    for (TFieldIterator<FProperty> It(Struct); It; ++It)
    {
        const FProperty* Property = *It;
        const FName MetaDataKey = FName(Property->GetAuthoredName());
        const FString MetaDataValue = StringTable->GetMetaData(TextKey, MetaDataKey);
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructValuePtr);
        
        if (!MetaDataValue.IsEmpty())
        {
            Property->ImportText_Direct(*MetaDataValue, ValuePtr, nullptr, PPF_Delimited);
            ChangedProperties++;
        }

        else
        {
            // Clear the value if it's not set in the metadata.
            Property->ClearValue(ValuePtr);
        }
    }
    
    return ChangedProperties;
}

bool FPSIStringTableUtilsHelper::SetStringTableEntryMetaDataFromStruct(const FStringTablePtr& StringTable, const FTextKey& TextKey,
    const UScriptStruct* Struct, const void* StructValuePtr)
{
    if (Struct == nullptr || StructValuePtr == nullptr || TextKey.IsEmpty() || !StringTable.IsValid())
    {
        return false;
    }
    
    bool bChanged = false;

    // The owner asset is modified once, right before the first change.
    const auto PrepareChange = [&bChanged, &StringTable]()
    {
#if WITH_EDITOR
        if (!bChanged)
        {
            PreStringTableChange(StringTable);
        }
#endif
        bChanged = true;
    };

    for (TFieldIterator<FProperty> It(Struct); It; ++It)
    {
        const FProperty* Property = *It;
        const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructValuePtr);
            
        const FName MetaDataKey = FName(Property->GetAuthoredName());
            
        // We compare the property value with its default value and decide whether we add the value to the 
        // metadata or remove it.
        //
        void* DefaultValuePtr = Property->AllocateAndInitializeValue();
        const bool bDiffersFromDefault = !Property->Identical(ValuePtr, DefaultValuePtr);
        Property->DestroyAndFreeValue(DefaultValuePtr);
        
        if (bDiffersFromDefault)
        {
            FString MetaDataValue;
            if (Property->ExportText_Direct(MetaDataValue, ValuePtr, nullptr, nullptr, PPF_Delimited | PPF_ExternalEditor))
            {
                PrepareChange();
                StringTable->SetMetaData(TextKey, MetaDataKey, MetaDataValue);
            }
        }

        else if (!StringTable->GetMetaData(TextKey, MetaDataKey).IsEmpty())
        {
            PrepareChange();
            StringTable->RemoveMetaData(TextKey, MetaDataKey);
        }
    }
    
#if WITH_EDITOR
    if (bChanged)
    {
        NotifyStringTableChanged(StringTable);
    }
#endif
    
    return bChanged;
}

bool FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetaData(const FStringTableConstPtr& StringTable, const FTextKey& Key)
{
    bool bHasMetaData = false;
    StringTable->EnumerateMetaData(Key, [&](const FName&, const FString&)
    {
        bHasMetaData = true;
        return false;
    });

    return bHasMetaData;
}

FString FPSIStringTableUtilsHelper::ExportStringTableToCSVString(const FStringTableConstPtr& StringTable)
{
    if (!StringTable.IsValid())
    {
        return FString();
    }

#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
    return StringTable->ExportStringsToCSVString();
#else
    // Older engine versions cannot export a string table to a CSV string, so the CSV is assembled here.
    // It uses the same format as the CSV files of the engine. Entries have no dev notes before UE 5.8, so there is
    // no dev notes column.
    //

    // Every field is quoted, with escaped special characters and doubled quotes.
    const auto MakeField = [](const FString& Value)
    {
        return FString::Printf(TEXT("\"%s\""), *Value.ReplaceCharWithEscapedChar().Replace(TEXT("\""), TEXT("\"\"")));
    };

    TArray<TPair<FString, FString>> Entries;
    StringTable->EnumerateSourceStrings([&Entries](const FString& Key, const FString& SourceString)
    {
        Entries.Emplace(Key, SourceString);
        return true;
    });

    // Each metadata ID used by any entry gets its own column.
    TArray<FName> MetaDataIds;
    for (const TPair<FString, FString>& Entry : Entries)
    {
        StringTable->EnumerateMetaData(Entry.Key, [&MetaDataIds](FName MetaDataId, const FString&)
        {
            MetaDataIds.AddUnique(MetaDataId);
            return true;
        });
    }

    TArray<FString> Lines;
    Lines.Reserve(Entries.Num() + 1);

    TArray<FString> HeaderFields = { FString(TEXT("Key")), FString(TEXT("SourceString")) };
    for (const FName MetaDataId : MetaDataIds)
    {
        HeaderFields.Add(MetaDataId.ToString());
    }
    Lines.Add(FString::Join(HeaderFields, TEXT(",")));

    for (const TPair<FString, FString>& Entry : Entries)
    {
        TArray<FString> Fields = { MakeField(Entry.Key), MakeField(Entry.Value) };
        for (const FName MetaDataId : MetaDataIds)
        {
            Fields.Add(MakeField(StringTable->GetMetaData(Entry.Key, MetaDataId)));
        }
        Lines.Add(FString::Join(Fields, TEXT(",")));
    }

    return FString::Join(Lines, TEXT("\n")) + TEXT("\n");
#endif
}

bool FPSIStringTableUtilsHelper::ExportStringTableToCSVFile(const FStringTableConstPtr& StringTable, const FString& FilePath)
{
    if (!StringTable.IsValid())
    {
        return false;
    }

#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
    return StringTable->ExportStringsToCSVFile(FilePath);
#else
    return StringTable->ExportStrings(FilePath);
#endif
}

bool FPSIStringTableUtilsHelper::ImportStringTableFromCSVString(const FStringTablePtr& StringTable, const FString& CSVString,
    const TCHAR* ErrorContext)
{
    if (!StringTable.IsValid())
    {
        return false;
    }

#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
#if WITH_EDITOR
    PreStringTableChange(StringTable);
#endif
    const bool bImported = StringTable->ImportStringsFromCSVString(CSVString, ErrorContext);
#if WITH_EDITOR
    NotifyStringTableChanged(StringTable);
#endif
    return bImported;
#else
    // Older engine versions cannot import a string table from a CSV string, so the CSV is read here.
    // It accepts the same format as the CSV files of the engine. Entries have no dev notes before UE 5.8, so a dev
    // notes column is ignored.
    //

    const FCsvParser Parser(CSVString);
    const FCsvParser::FRows& Rows = Parser.GetRows();
    if (Rows.Num() < 2)
    {
        UE_LOG(LogPSIStringTableUtils, Warning, TEXT("[FPSIStringTableUtilsHelper::ImportStringTableFromCSVString] Failed to import '%s'. It needs a header row and at least one entry row."), ErrorContext);
        return false;
    }

    const TArray<const TCHAR*>& Header = Rows[0];
    const auto FindColumn = [&Header](const TCHAR* ColumnName)
    {
        return Header.IndexOfByPredicate([ColumnName](const TCHAR* Cell)
        {
            return FCString::Stricmp(Cell, ColumnName) == 0;
        });
    };

    const int32 KeyColumn = FindColumn(TEXT("Key"));
    const int32 SourceStringColumn = FindColumn(TEXT("SourceString"));
    const int32 DevNotesColumn = FindColumn(TEXT("DevNotes"));
    if (KeyColumn == INDEX_NONE || SourceStringColumn == INDEX_NONE)
    {
        UE_LOG(LogPSIStringTableUtils, Warning, TEXT("[FPSIStringTableUtilsHelper::ImportStringTableFromCSVString] Failed to import '%s'. It needs a 'Key' and a 'SourceString' column."), ErrorContext);
        return false;
    }

    // All other named columns hold metadata.
    TArray<TPair<int32, FName>> MetaDataColumns;
    for (int32 Column = 0; Column < Header.Num(); ++Column)
    {
        const FName MetaDataId(Header[Column]);
        if (Column != KeyColumn && Column != SourceStringColumn && Column != DevNotesColumn && !MetaDataId.IsNone())
        {
            MetaDataColumns.Emplace(Column, MetaDataId);
        }
    }

    // Returns the unescaped value of a cell, or an empty string for a cell the row does not have.
    const auto GetCell = [](const TArray<const TCHAR*>& Row, const int32 Column)
    {
        return Row.IsValidIndex(Column) ? FString(Row[Column]).ReplaceEscapedCharWithChar() : FString();
    };

#if WITH_EDITOR
    PreStringTableChange(StringTable);
#endif
    StringTable->ClearSourceStrings();

    for (int32 RowIndex = 1; RowIndex < Rows.Num(); ++RowIndex)
    {
        const TArray<const TCHAR*>& Row = Rows[RowIndex];

        // 'SetSourceString' asserts on an empty key, so rows without a key are skipped.
        const FString Key = GetCell(Row, KeyColumn);
        if (Key.IsEmpty())
        {
            continue;
        }

        StringTable->SetSourceString(Key, GetCell(Row, SourceStringColumn));

        for (const TPair<int32, FName>& MetaDataColumn : MetaDataColumns)
        {
            const FString MetaDataValue = GetCell(Row, MetaDataColumn.Key);
            if (!MetaDataValue.IsEmpty())
            {
                StringTable->SetMetaData(Key, MetaDataColumn.Value, MetaDataValue);
            }
        }
    }

#if WITH_EDITOR
    NotifyStringTableChanged(StringTable);
#endif
    return true;
#endif
}

bool FPSIStringTableUtilsHelper::ImportStringTableFromCSVFile(const FStringTablePtr& StringTable, const FString& FilePath)
{
    if (!StringTable.IsValid())
    {
        return false;
    }

    // A missing file would otherwise mark the String Table dirty without changing it.
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogPSIStringTableUtils, Warning, TEXT("[FPSIStringTableUtilsHelper::ImportStringTableFromCSVFile] Failed to import '%s'. The file does not exist."), *FilePath);
        return false;
    }

#if WITH_EDITOR
    PreStringTableChange(StringTable);
#endif

#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
    const bool bImported = StringTable->ImportStringsFromCSVFile(FilePath);
#else
    const bool bImported = StringTable->ImportStrings(FilePath);
#endif

#if WITH_EDITOR
    NotifyStringTableChanged(StringTable);
#endif
    return bImported;
}

#if WITH_EDITOR
void FPSIStringTableUtilsHelper::PreStringTableChange(const FStringTableConstPtr& StringTable)
{
    UStringTable* OwnerAsset = StringTable.IsValid() ? StringTable->GetOwnerAsset() : nullptr;
    if (OwnerAsset != nullptr)
    {
        OwnerAsset->Modify();
    }
}

void FPSIStringTableUtilsHelper::NotifyStringTableChanged(const FStringTableConstPtr& StringTable)
{
    if (!StringTable.IsValid())
    {
        return;
    }

    UStringTable* OwnerAsset = StringTable->GetOwnerAsset();
    if (OwnerAsset == nullptr)
    {
        return;
    }

    // The native String Table editor does not react to changes made through this helper on its own, so any editor
    // that currently has the asset open needs to be told to refresh explicitly. That logic lives in
    // 'PSIStringTableUtilsEditor', an editor-only module this one may not depend on, so it is reached through a
    // delegate instead.
    //
    IPSIStringTableUtilsModule::Get().OnStringTableChangedExternally.ExecuteIfBound(OwnerAsset);
}
#endif
