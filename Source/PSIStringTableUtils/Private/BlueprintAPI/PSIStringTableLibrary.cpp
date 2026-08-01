// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableLibrary.h"
#include "PSIStringTableHelper.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "UObject/TextProperty.h"
#include "PSIEngineVersionComparison.h"

#define LOCTEXT_NAMESPACE "PSIStringTableUtils"

void UPSIStringTableLibrary::GetStringTableEntryMetaDataAsStruct(const FName, const FString&, FGenericStruct&, int32&)
{
    // We should never hit this! stubs to avoid NoExport on the class.
    checkNoEntry();
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execGetStringTableEntryMetaDataAsStruct)
{
    execPopulateStructWithStringTableEntryMetaData(Context, Stack, RESULT_PARAM);
}

void UPSIStringTableLibrary::PopulateStructWithStringTableEntryMetaData(const FName, const FString&, FGenericStruct&, int32&)
{
    // We should never hit this! stubs to avoid NoExport on the class.
    checkNoEntry();
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execPopulateStructWithStringTableEntryMetaData)
{
    P_GET_PROPERTY(FNameProperty, TableId)
    P_GET_PROPERTY(FStrProperty, Key)
    
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
            EBlueprintExceptionType::AbortExecution,
            LOCTEXT("InvalidStructValueWarning", "Invalid struct value provided as wildcard")
        );
    
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
    }
    
    else
    {
        P_NATIVE_BEGIN;
        RetrievedValues = FPSIStringTableHelper::GetStringTableEntryMetaDataAsStruct(
                 FStringTableRegistry::Get().FindStringTable(TableId), Key, ValueProp->Struct, ValuePtr);
        P_NATIVE_END;
    }
}

#if WITH_EDITOR
bool UPSIStringTableLibrary::SetStringTableSourceString(const FName TableId, const FString& Key,
    const FString& InSourceString, bool bPreserveData)
{
    if (Key.IsEmpty())
    {
        return false;
    }

    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return false;
    }
    
    const FTextKey TextKey(Key);
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
            
            FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
            return true;
        }
    }

#if WITH_EDITORONLY_DATA
    StringTable->SetSourceString(TextKey, InSourceString, FString());
#else
    StringTable->SetSourceString(TextKey, InSourceString);
#endif
    
    FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
    
    return true;
}

bool UPSIStringTableLibrary::RemoveStringTableEntry(const FName TableId, const FString& Key)
{
    if (Key.IsEmpty())
    {
        return false;
    }

    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return false;
    }
    
    const FTextKey TextKey(Key);
    if (StringTable->FindEntry(TextKey).IsValid())
    {
        StringTable->RemoveSourceString(TextKey);
        FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
        return true;
    }
    
    return false;
}

bool UPSIStringTableLibrary::SetStringTableEntryMetaDataFromStruct(const FName, const FString&, const FGenericStruct&)
{
    // We should never hit this! stubs to avoid NoExport on the class.
    checkNoEntry();
    return false;
}

DEFINE_FUNCTION(UPSIStringTableLibrary::execSetStringTableEntryMetaDataFromStruct)
{
    P_GET_PROPERTY(FNameProperty, TableId)
    P_GET_PROPERTY(FStrProperty, Key)
    
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
        *static_cast<bool*>(RESULT_PARAM) = FPSIStringTableHelper::SetStringTableEntryMetaDataFromStruct(
            FStringTableRegistry::Get().FindMutableStringTable(TableId), Key, ValueProp->Struct, ValuePtr);
        P_NATIVE_END;
    }
}

bool UPSIStringTableLibrary::SetStringTableEntryMetaData(const FName TableId, const FString& Key,
                                                         const FName MetaDataId, FString MetaDataValue)
{
    if (Key.IsEmpty())
    {
        return false;
    }

    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey(Key);
    const FStringTableEntryConstPtr StringTableEntry = StringTable->FindEntry(TextKey);
    if (!StringTableEntry.IsValid())
    {
        return false;
    }
    
    StringTable->SetMetaData(TextKey, MetaDataId, MetaDataValue);
    FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
    
    return true;
}

bool UPSIStringTableLibrary::RemoveStringTableEntryMetaData(const FName TableId, const FString& Key,
                                                            const FName MetaDataId)
{
    if (Key.IsEmpty())
    {
        return false;
    }
        
    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey(Key);
    if (!StringTable->GetMetaData(TextKey, MetaDataId).IsEmpty()) 
    {
        StringTable->RemoveMetaData(Key, MetaDataId);
        FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
        
        return true;
    }

    return false;
}

bool UPSIStringTableLibrary::ClearMetaDataFromStringTableEntry(const FName TableId, const FString& Key)
{
    if (Key.IsEmpty())
    {
        return false;
    }
        
    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return false;
    }

    const FTextKey TextKey(Key);
    if (FPSIStringTableHelper::DoesStringTableEntryHasMetaData(StringTable, TextKey))
    {
        StringTable->ClearMetaData(TextKey);
        FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
        return true;
    }

    return false;
}

void UPSIStringTableLibrary::ClearStringTableMetaData(const FName TableId)
{
    const FStringTablePtr StringTable = FStringTableRegistry::Get().FindMutableStringTable(TableId);
    if (!StringTable.IsValid())
    {
        return;
    }
    
    StringTable->ClearMetaData();
    
    // We just assume that some metadata existed and has been removed.
    FPSIStringTableHelper::NotifyStringTableChanged(StringTable);
}

#endif

#undef LOCTEXT_NAMESPACE
