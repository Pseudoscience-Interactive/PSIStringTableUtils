// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableLibrary.h"
#include "PSIEngineVersionComparison.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableUtilsHelper.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "UObject/TextProperty.h"

#define LOCTEXT_NAMESPACE "PSIStringTableUtils"

bool UPSIStringTableLibrary::IsStringTableRegistered(FPSIStringTableId TableId)
{
    return TableId.GetStringTable().IsValid();
}

bool UPSIStringTableLibrary::IsStringTableEntryRegistered(FPSIStringTableEntryHandle Entry)
{
    return Entry.GetStringTableEntry().IsValid();
}

FString UPSIStringTableLibrary::GetStringTableNamespace(FPSIStringTableId TableId)
{
    const FStringTableConstPtr StringTable = TableId.GetStringTable();
    if (!StringTable.IsValid())
    {
        return FString();
    }
    
    return StringTable->GetNamespace();
}

FString UPSIStringTableLibrary::GetStringTableSourceString(FPSIStringTableEntryHandle Entry)
{
    FString SourceString;

    const FStringTableConstPtr StringTable = Entry.GetStringTable();
    if (StringTable.IsValid())
    {
        StringTable->GetSourceString(Entry.GetTextKey(), SourceString);
    }

    return SourceString;
}

FString UPSIStringTableLibrary::GetStringTableEntryMetaData(FPSIStringTableEntryHandle Entry, const FName MetaDataId)
{
    const FStringTableConstPtr StringTable = Entry.GetStringTable();
    if (StringTable.IsValid())
    {
        return StringTable->GetMetaData(Entry.GetTextKey(), MetaDataId);
    }

    return FString();
}

TArray<FString> UPSIStringTableLibrary::GetKeysFromStringTable(FPSIStringTableId TableId)
{
    TArray<FString> KeysFromStringTable;

    const FStringTableConstPtr StringTable = TableId.GetStringTable();
    if (StringTable.IsValid())
    {
        StringTable->EnumerateSourceStrings([&](const FString& InKey, const FString&) -> bool
        {
            KeysFromStringTable.Add(InKey);
            return true;
        });
    }

    return KeysFromStringTable;
}

TArray<FName> UPSIStringTableLibrary::GetMetaDataIdsFromStringTableEntry(FPSIStringTableEntryHandle Entry)
{
    TArray<FName> MetaDataIdsFromStringTable;

    const FStringTableConstPtr StringTable = Entry.GetStringTable();
    if (StringTable.IsValid())
    {
        StringTable->EnumerateMetaData(Entry.GetTextKey(), [&](FName InMetaDataId, const FString&) -> bool
        {
            MetaDataIdsFromStringTable.Add(InMetaDataId);
            return true; // continue enumeration
        });
    }

    return MetaDataIdsFromStringTable;
}

bool UPSIStringTableLibrary::ExportStringTableToCSVString(FPSIStringTableId TableId, FString& OutCSVString)
{
    if (const FStringTableConstPtr StringTable = TableId.GetStringTable())
    {
        OutCSVString = StringTable->ExportStringsToCSVString();
        return true;
    }

    return false;
}

bool UPSIStringTableLibrary::ExportStringTableToCSVFile(FPSIStringTableId TableId, const FString& Filename)
{
    if (const FStringTableConstPtr StringTable = TableId.GetStringTable())
    {
        return StringTable->ExportStringsToCSVFile(Filename);
    }

    return false;
}

bool UPSIStringTableLibrary::ImportStringTableFromCSVString(FPSIStringTableId TableId, const FString& CSVString)
{
    if (const FStringTablePtr StringTable = TableId.GetStringTable())
    {
        return StringTable->ImportStringsFromCSVString(CSVString, *TableId.ToString());
    }

    return false;
}

bool UPSIStringTableLibrary::ImportStringTableFromCSVFile(FPSIStringTableId TableId, const FString& Filename)
{
    if (const FStringTablePtr StringTable = TableId.GetStringTable())
    {
        return StringTable->ImportStringsFromCSVFile(Filename);
    }

    return false;
}

void UPSIStringTableLibrary::GetStringTableEntryMetaDataAsStruct(FPSIStringTableEntryHandle, FGenericStruct&, int32&)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execGetStringTableEntryMetaDataAsStruct)
{
    execPopulateStructWithStringTableEntryMetaData(Context, Stack, RESULT_PARAM);
}

void UPSIStringTableLibrary::PopulateStructWithStringTableEntryMetaData(FPSIStringTableEntryHandle, FGenericStruct&, int32&)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execPopulateStructWithStringTableEntryMetaData)
{
    P_GET_STRUCT(FPSIStringTableEntryHandle, Entry);
    
    // Read wildcard Value input.
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FStructProperty>(nullptr);
    const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
    void* ValuePtr = Stack.MostRecentPropertyAddress;

    P_GET_PROPERTY_REF(FIntProperty, RetrievedValues)
    
    P_FINISH;
    
    if (ValueProp == nullptr || ValuePtr == nullptr)
    {
        const FBlueprintExceptionInfo ExceptionInfo(
            EBlueprintExceptionType::AccessViolation,
            LOCTEXT("InvalidStructValueWarning", "Invalid struct value provided as wildcard")
        );
    
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
    }
    
    else
    {
        P_NATIVE_BEGIN;
        RetrievedValues = FPSIStringTableUtilsHelper::GetStringTableEntryMetaDataAsStruct(
                 Entry.GetStringTable(), Entry.GetTextKey(), ValueProp->Struct, ValuePtr);
        P_NATIVE_END;
    }
}

#if WITH_EDITOR

bool UPSIStringTableLibrary::SetStringTableSourceString(FPSIStringTableEntryHandle Entry,
    const FString& InSourceString, bool bPreserveData)
{
    if (Entry.IsNone())
    {
        return false;
    }

    const FStringTablePtr StringTable = Entry.GetStringTable();
    if (!StringTable.IsValid())
    {
        return false;
    }
    
    const FTextKey TextKey = Entry.GetTextKey();
    if (bPreserveData)
    {
        const FStringTableEntryConstPtr StringTableEntry = StringTable->FindEntry(TextKey);
        if (StringTable.IsValid())
        {
            TArray<TTuple<FName, FString>> MetaData;
            StringTable->EnumerateMetaData(TextKey, [&MetaData](FName MetaDataId, const FString& MetaDataValue)
            {
                MetaData.Emplace(MetaDataId, MetaDataValue);
                
                // continue
                return true;
            });

#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0) && WITH_EDITORONLY_DATA
            const FString DevNotes = StringTableEntry->GetDevNotes();
            StringTable->SetSourceString(TextKey, InSourceString, DevNotes);
#else
            StringTable->SetSourceString(TextKey, InSourceString);
#endif
            
            for (const TTuple<FName, FString>& Tuple : MetaData)
            {
                StringTable->SetMetaData(TextKey, Tuple.Key, Tuple.Value);
            }
            
            FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
            return true;
        }
    }

#if WITH_EDITORONLY_DATA
    StringTable->SetSourceString(TextKey, InSourceString, FString());
#else
    StringTable->SetSourceString(TextKey, InSourceString);
#endif
    
    FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
    
    return true;
}

bool UPSIStringTableLibrary::RemoveStringTableEntry(FPSIStringTableEntryHandle Entry)
{
    if (Entry.IsNone())
    {
        return false;
    }

    const FStringTablePtr StringTable = Entry.GetStringTable();
    if (!StringTable.IsValid())
    {
        return false;
    }
    
    const FTextKey TextKey = Entry.GetTextKey();
    if (StringTable->FindEntry(TextKey).IsValid())
    {
        StringTable->RemoveSourceString(TextKey);
        FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
        return true;
    }
    
    return false;
}

bool UPSIStringTableLibrary::SetStringTableEntryMetaDataFromStruct(FPSIStringTableEntryHandle, const FGenericStruct&)
{
    checkNoEntry();
    return false;
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execSetStringTableEntryMetaDataFromStruct)
{
    P_GET_STRUCT(FPSIStringTableEntryHandle, Entry);
    
    // Read wildcard Value input.
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FStructProperty>(nullptr);
    const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
    const void* ValuePtr = Stack.MostRecentPropertyAddress;
    
    P_FINISH;
    
    if (ValueProp == nullptr || ValuePtr == nullptr)
    {
        const FBlueprintExceptionInfo ExceptionInfo(
            EBlueprintExceptionType::AbortExecution,
            LOCTEXT("InvalidStructValueWarning", "Invalid struct value provided as wildcard")
        );
    
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
    }
    
    else
    {
        P_NATIVE_BEGIN;
        *static_cast<bool*>(RESULT_PARAM) = FPSIStringTableUtilsHelper::SetStringTableEntryMetaDataFromStruct(
            Entry.GetStringTable(), Entry.GetTextKey(), ValueProp->Struct, ValuePtr);
        P_NATIVE_END;
    }
}

bool UPSIStringTableLibrary::SetStringTableEntryMetaData(FPSIStringTableEntryHandle Entry,
                                                         const FName MetaDataId, FString MetaDataValue)
{
    if (Entry.IsNone())
    {
        return false;
    }

    const FStringTablePtr StringTable = Entry.GetStringTable();
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey = Entry.GetTextKey();
    const FStringTableEntryConstPtr StringTableEntry = StringTable->FindEntry(TextKey);
    if (!StringTableEntry.IsValid())
    {
        return false;
    }
    
    StringTable->SetMetaData(TextKey, MetaDataId, MetaDataValue);
    FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
    
    return true;
}

bool UPSIStringTableLibrary::RemoveStringTableEntryMetaData(FPSIStringTableEntryHandle Entry,
                                                            const FName MetaDataId)
{
    if (Entry.IsNone())
    {
        return false;
    }
        
    const FStringTablePtr StringTable = Entry.GetStringTable();
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey = Entry.GetTextKey();
    if (!StringTable->GetMetaData(TextKey, MetaDataId).IsEmpty()) 
    {
        StringTable->RemoveMetaData(TextKey, MetaDataId);
        FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
        
        return true;
    }

    return false;
}

bool UPSIStringTableLibrary::ClearMetaDataFromStringTableEntry(FPSIStringTableEntryHandle Entry)
{
    if (Entry.IsNone())
    {
        return false;
    }
        
    const FStringTablePtr StringTable = Entry.GetStringTable();
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey = Entry.GetTextKey();
    if (FPSIStringTableUtilsHelper::DoesStringTableEntryHasMetaData(StringTable, TextKey))
    {
        StringTable->ClearMetaData(TextKey);
        FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
        return true;
    }

    return false;
}

void UPSIStringTableLibrary::ClearStringTableMetaData(FPSIStringTableId TableId)
{
    const FStringTablePtr StringTable = TableId.GetStringTable();
    if (!StringTable.IsValid())
    {
        return;
    }
    
    StringTable->ClearMetaData();
    
    // We just assume that some metadata existed and has been removed.
    FPSIStringTableUtilsHelper::NotifyStringTableChanged(StringTable);
}

void UPSIStringTableLibrary::SetStringTableNamespace(FPSIStringTableId TableId, const FString& Namespace)
{
    const FStringTablePtr StringTable = TableId.GetStringTable();
    if (!StringTable.IsValid())
    {
        return;
    }
    
    StringTable->SetNamespace(Namespace);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////
// FPSIStringTableId

bool UPSIStringTableLibrary::IsValidStringTableId(const FPSIStringTableId& Value)
{
    return Value.GetStringTable().IsValid();
}

FPSIStringTableId UPSIStringTableLibrary::GetStringTableId(const UStringTable* StringTable)
{
    return StringTable != nullptr ? StringTable->GetStringTableId() : FName();
}

FPSIStringTableId UPSIStringTableLibrary::MakeLiteralStringTableId(FPSIStringTableId Value)
{
    return Value;
}

bool UPSIStringTableLibrary::EqualEqual_StringTableId(const FPSIStringTableId A, FPSIStringTableId B)
{
    return A == B;
}

bool UPSIStringTableLibrary::NotEqual_StringTableId(FPSIStringTableId A, FPSIStringTableId B)
{
    return A != B;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// FPSIStringTableEntryHandle

bool UPSIStringTableLibrary::IsValidStringTableEntryHandle(const FPSIStringTableEntryHandle& Entry)
{
    return Entry.GetStringTableEntry().IsValid();
}

FPSIStringTableEntryHandle UPSIStringTableLibrary::GetStringTableEntryHandleFromText(const FText& Text)
{
    return FPSIStringTableEntryHandle(Text);
}

FPSIStringTableEntryHandle UPSIStringTableLibrary::MakeLiteralStringTableEntryHandle(FPSIStringTableEntryHandle Value)
{
    return Value;
}

bool UPSIStringTableLibrary::EqualEqual_StringTableEntryHandle(FPSIStringTableEntryHandle A, FPSIStringTableEntryHandle B)
{
    return A == B;
}

bool UPSIStringTableLibrary::NotEqual_StringTableEntryHandle(FPSIStringTableEntryHandle A, FPSIStringTableEntryHandle B)
{
    return A != B;
}

#undef LOCTEXT_NAMESPACE
