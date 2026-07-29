// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableHelper.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "UObject/Class.h"
#include "UObject/FieldIterator.h"

int32 FPSIStringTableHelper::GetStringTableEntryMetaDataAsStruct(const FStringTableConstPtr& StringTable,
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

bool FPSIStringTableHelper::SetStringTableEntryMetaDataFromStruct(const FStringTablePtr& StringTable, const FTextKey& TextKey,
    const UScriptStruct* Struct, const void* StructValuePtr)
{
    if (Struct == nullptr || StructValuePtr == nullptr || TextKey.IsEmpty() || !StringTable.IsValid())
    {
        return false;
    }
    
    bool bChanged = false;
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
                StringTable->SetMetaData(TextKey, MetaDataKey, MetaDataValue);
                bChanged = true;
            }
        }
            
        else if (!StringTable->GetMetaData(TextKey, MetaDataKey).IsEmpty())
        {
            StringTable->RemoveMetaData(TextKey, MetaDataKey);
            bChanged = true;
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

bool FPSIStringTableHelper::DoesStringTableEntryHasMetaData(const FStringTableConstPtr& StringTable, const FTextKey& Key)
{
    bool bHasMetaData;
    StringTable->EnumerateMetaData(Key, [&](const FName&, const FString&)
    {
        bHasMetaData = true;
        return false;
    });
    
    return bHasMetaData;
}

#if WITH_EDITOR
void FPSIStringTableHelper::NotifyStringTableChanged(const FStringTableConstPtr& StringTable)
{
    if (!StringTable.IsValid())
    {
        return;
    }
    
    UStringTable* OwnerAsset = StringTable->GetOwnerAsset();
    if (OwnerAsset != nullptr)
    {
        OwnerAsset->Modify();
    }
}
#endif
