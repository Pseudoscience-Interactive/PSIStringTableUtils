// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIEngineVersionComparison.h"
#include "PSIStringTableId.h"
#include "PSIStringTableEntryHandle.generated.h"

class UStringTable;

/**
 * Handle to an Entry in a String Table.
 */
USTRUCT(BlueprintType, DisplayName = "PSI String Table Entry Handle")
struct PSISTRINGTABLEUTILS_API FPSIStringTableEntryHandle
{
    GENERATED_BODY()
    
    /** ID of the String Table. */
    UPROPERTY(Category = PSI, EditAnywhere, BlueprintReadWrite)
    FPSIStringTableId TableId;

    /** The text key of inside the String Table. */
    UPROPERTY(Category = PSI, EditAnywhere, BlueprintReadWrite)
    FString Key;
    
    FPSIStringTableEntryHandle() {}
    explicit FPSIStringTableEntryHandle(const FText& Text);
    
    /**
     * Creates a new instance of the 'FPSIStringTableEntryHandle' struct.
     * @param InTableId The ID of the String Table. 'FPSIStringTableId' will also be accepted due to its implicit 
     * conversion operators.
     * @param InKey String representation of the text key of the String Table entry.
     */ 
    FPSIStringTableEntryHandle(const FName& InTableId, const FString& InKey)
        : TableId(InTableId)
        , Key(InKey)
    {}
    
    /**
     * Creates a new instance of the 'FPSIStringTableEntryHandle' struct.
     * @param InTableId The ID of the String Table. 'FPSIStringTableId' will also be accepted due to its implicit 
     * conversion operators.
     * @param InKey The 'FTextKey' of the String Table entry.
     */ 
    FPSIStringTableEntryHandle(const FName& InTableId, const FTextKey& InKey)
        : TableId(InTableId)
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
        , Key(InKey.ToString())
#else
        , Key(InKey.GetChars())
#endif
    {}
    
    /** Returns true if this handle is specifically pointing to nothing */
    [[nodiscard]] bool IsNone() const
    {
        return TableId.IsNone() && Key.IsEmpty();
    }
    
    /** Returns the text key of this handle. */
    [[nodiscard]] FTextKey GetTextKey() const
    {
        return FTextKey(Key);
    }
    
    /** Resets all child properties. */
    void Reset()
    {
        TableId.Reset();
        Key.Reset();
    }
    
    /** Gets a pointer to the String Table this 'FPSIStringTableEntryHandle' is referring to. */
    [[nodiscard]] FStringTablePtr GetStringTable();
    
    /** Gets a pointer to the String Table this 'FPSIStringTableEntryHandle' is referring to. */
    [[nodiscard]] FStringTableConstPtr GetStringTable() const;
    
    /** Gets a pointer to the String Table Entry this 'FPSIStringTableEntryHandle' is referring to. */
    [[nodiscard]] FStringTableEntryConstPtr GetStringTableEntry() const;
    
    /** Gets the owner asset of the String Table. Note that not all valid String Tables have an 'UStringTable' owner.*/
    [[nodiscard]] UStringTable* GetStringTableOwnerAsset() const;
    
    /** Returns a string representation of this handle. */
    [[nodiscard]] FString ToString() const;

    /** Returns a debug string of this handle. */
    [[nodiscard]] FString ToDebugString() const;
    
    bool operator==(const FPSIStringTableEntryHandle& Other) const
    {
        return TableId == Other.TableId && Key == Other.Key;
    }

    bool operator!=(const FPSIStringTableEntryHandle& Other) const
    {
        return TableId != Other.TableId || Key != Other.Key;
    }
    
    void PostSerialize(const FArchive& Ar); 

    friend uint32 GetTypeHash(const FPSIStringTableEntryHandle& Handle)
    {
        uint32 Hash = 0;

        Hash = HashCombine(Hash, GetTypeHash(Handle.TableId));
        Hash = HashCombine(Hash, GetTypeHash(Handle.Key));
        return Hash;
    }
};

template<>
struct TStructOpsTypeTraits<FPSIStringTableEntryHandle> : TStructOpsTypeTraitsBase2<FPSIStringTableEntryHandle>
{
    enum
    {
        WithZeroConstructor = true,
        WithCopy = true,
        WithIdenticalViaEquality = true,
        WithPostSerialize = true,
    };
};
