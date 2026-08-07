// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableId.generated.h"

class UStringTable;

/**
 * ID for a String Table. Allows a String Table to be selected in the editor by using a dropdown.
 */
USTRUCT(BlueprintType, DisplayName = "PSI String Table ID")
struct PSISTRINGTABLEUTILS_API FPSIStringTableId
{
    GENERATED_BODY()
    
    /** Name of the ID of the String Table. */
    UPROPERTY(Category = PSI, EditAnywhere, BlueprintReadWrite)
    FName Name;
    
    /** Constructors, one for each 'FName' constructor */
    FPSIStringTableId() {}
    FPSIStringTableId(FName InName) : Name(InName) {}
    FPSIStringTableId(EName InName) : Name(FName(InName)) {}
    FPSIStringTableId(const WIDECHAR* InName) : Name(FName(InName)) {}
    FPSIStringTableId(const ANSICHAR* InName) : Name(FName(InName)) {}

    /** Implicit conversion operator to 'FName'. */
    operator FName&() { return Name; }

    /** Implicit conversion operator to 'FName'. */
    operator const FName&() const { return Name; }

    bool operator==(const FPSIStringTableId& Other) const
    {
        return Name == Other.Name;
    }

    bool operator!=(const FPSIStringTableId& Other) const
    {
        return Name != Other.Name;
    }

    /** Returns true if this is a valid Type */
    [[nodiscard]] bool IsNone() const
    {
        return Name == NAME_None;
    }

    /** Returns string version of this Type */
    [[nodiscard]] FString ToString() const
    {
        return Name.ToString();
    }
    
    /** Resets the 'Name' to 'None'. */
    void Reset()
    {
        Name = NAME_None;
    }
    
    /** Gets a pointer to the String Table this 'FPSIStringTableEntryHandle' is referring to. */
    [[nodiscard]] FStringTablePtr GetStringTable();
    
    /** Gets a pointer to the String Table this 'FPSIStringTableEntryHandle' is referring to. */
    [[nodiscard]] FStringTableConstPtr GetStringTable() const;
    
    /** Gets the owner asset of the String Table. Note that not all valid String Tables have an 'UStringTable' owner.*/
    [[nodiscard]] UStringTable* GetStringTableOwnerAsset() const;
    
    bool ExportTextItem(FString& ValueStr, FPSIStringTableId const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;
    bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);
    bool SerializeFromMismatchedTag(FPropertyTag const& Tag, FStructuredArchive::FSlot Slot);

    friend uint32 GetTypeHash(const FPSIStringTableId& Key)
    {
        return GetTypeHash(Key.Name);
    }
};

template<>
struct TStructOpsTypeTraits<FPSIStringTableId> : TStructOpsTypeTraitsBase2<FPSIStringTableId>
{
    enum
    {
        WithZeroConstructor = true,
        WithCopy = true,
        WithIdenticalViaEquality = true,
        WithExportTextItem = true,
        WithImportTextItem = true,
        WithStructuredSerializeFromMismatchedTag = true,
    };
};