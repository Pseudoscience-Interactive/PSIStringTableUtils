// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableEntryHandle.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTable.h"

FStringTablePtr FPSIStringTableEntryHandle::GetStringTable()
{
    return TableId.GetStringTable();
}

FStringTableConstPtr FPSIStringTableEntryHandle::GetStringTable() const
{    
    return TableId.GetStringTable();
}

FStringTableEntryConstPtr FPSIStringTableEntryHandle::GetStringTableEntry() const
{
    if (Key.IsEmpty())
    {
        return nullptr;
    }
    
    const FStringTableConstPtr StringTable = GetStringTable();
    if (!StringTable.IsValid())
    {
        return nullptr;
    }
    
    return StringTable->FindEntry(FTextKey(Key));
}

UStringTable* FPSIStringTableEntryHandle::GetStringTableOwnerAsset() const
{
    return TableId.GetStringTableOwnerAsset();
}

FString FPSIStringTableEntryHandle::ToString() const
{
    if (!IsNone())
    {
        return FString::Printf(TEXT("%s:%s"), *TableId.ToString(), *Key);
    }

    return FString();
}

FString FPSIStringTableEntryHandle::ToDebugString() const
{
    if (TableId.IsNone())
    {
        return FString::Printf(TEXT("No String Table ID Specified, Key: %s"), *Key);
    }

    return FString::Printf(TEXT("Table ID: %s, Key: %s"), *TableId.ToString(), *Key);
}

void FPSIStringTableEntryHandle::PostSerialize(const FArchive& Ar)
{
    if (Ar.IsSaving() && !IsNone())
    {
        UStringTable* OwnerAsset = GetStringTableOwnerAsset();
        if (OwnerAsset != nullptr)
        {
            Ar.MarkSearchableName(OwnerAsset, *Key);
        }
    }
}
