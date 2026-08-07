// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableId.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"

FStringTablePtr FPSIStringTableId::GetStringTable()
{
    if (Name.IsNone())
    {
        return nullptr;
    }
    
    return FStringTableRegistry::Get().FindMutableStringTable(Name);
}

FStringTableConstPtr FPSIStringTableId::GetStringTable() const
{    
    if (Name.IsNone())
    {
        return nullptr;
    }
    
    return FStringTableRegistry::Get().FindStringTable(Name);
    
}

UStringTable* FPSIStringTableId::GetStringTableOwnerAsset() const
{
    if (Name.IsNone())
    {
        return nullptr;
    }
    
    const FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(Name);
    if (StringTable.IsValid())
    {
        UStringTable* OwnerAsset = StringTable->GetOwnerAsset();
        if (OwnerAsset != nullptr)
        {
            return OwnerAsset;
        }
    }
    
    return nullptr;
}

bool FPSIStringTableId::ExportTextItem(FString& ValueStr, FPSIStringTableId const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
    if (!(PortFlags & PPF_Delimited))
    {
        ValueStr += ToString();
    }
    else
    {
        ValueStr += FString::Printf(TEXT("\"%s\""), *ToString().ReplaceCharWithEscapedChar());
    }

    return true;
}

bool FPSIStringTableId::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
    // This handles both quoted and unquoted
    FString ImportedString = TEXT("");
    const TCHAR* NewBuffer = FPropertyHelpers::ReadToken(Buffer, ImportedString, true);

    if (!NewBuffer)
    {
        return false;
    }

    *this = FPSIStringTableId(*ImportedString);
    Buffer = NewBuffer;

    return true;
}

bool FPSIStringTableId::SerializeFromMismatchedTag(FPropertyTag const& Tag, FStructuredArchive::FSlot Slot)
{
    if (Tag.Type == NAME_NameProperty)
    {
        FName InName;
        Slot << InName;
        *this = FPSIStringTableId(InName);
        return true;
    }
    
    if (Tag.Type == NAME_StrProperty)
    {
        FString InString;
        Slot << InString;
        *this = FPSIStringTableId(*InString);
        return true;
    }

    return false;
}