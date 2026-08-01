// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PSIStringTableLibrary.generated.h"

struct FGenericStruct;

/**
 * Blueprint API to modify String Tables and to make it easier to retrieve their metadata.
 */
UCLASS(DisplayName="PSI String Table Library")
class UPSIStringTableLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
    /**
     * Gets the metadata of a specific string table entry as property values of a given struct. Each property of the 
     * struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if the 
     * authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param StructValue The output struct to store the metadata values.
     * @param RetrievedValues The total number of metadata values retrieved.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Get String Table Entry Meta-Data as Stuct", CustomStructureParam = "StructValue", AdvancedDisplay = RetrievedValues))
    static void GetStringTableEntryMetaDataAsStruct(const FName TableId, const FString& Key, FGenericStruct& StructValue, int32& RetrievedValues);
    DECLARE_FUNCTION(execGetStringTableEntryMetaDataAsStruct);

    /**
     * Retrieves the metadata of a specific string table entry as property values of a given struct. Each property of 
     * the struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if 
     * the authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param StructValueRef The output struct to store the metadata values.
     * @param RetrievedValues The total number of metadata values retrieved.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Populate Struct with String Table Entry Meta-Data", CustomStructureParam = "StructValueRef", AdvancedDisplay = RetrievedValues))
    static void PopulateStructWithStringTableEntryMetaData(const FName TableId, const FString& Key, UPARAM(ref) FGenericStruct& StructValueRef, int32& RetrievedValues);
    DECLARE_FUNCTION(execPopulateStructWithStringTableEntryMetaData);
    
#if WITH_EDITOR
    /**
     * Sets or adds the source string for a specific text key in the specified string table. 
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param InSourceString The source string to be set for the string table entry.
     * @param bPreserveData Whether any existing data for that entry should be preserved.
     * @return True, if the source string was successfully set; False, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Set String Table Source String", AdvancedDisplay = bPreserveData))
    static bool SetStringTableSourceString(const FName TableId, const FString& Key, const FString& InSourceString, bool bPreserveData);

    /**
     * Removes a source string associated with a specified key from the given string table. This also includes all
     * metadata associated with that entry.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry to remove.
     * @return True, if the entry was successfully removed; False, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Remove String Table Entry"))
    static bool RemoveStringTableEntry(const FName TableId, const FString& Key);
    
    /** 
     * Sets the metadata of a specific string table entry from the property values of a given struct. Each property 
     * of the struct is mapped to a metadata, where the property name corresponds to the metadata ID, and the property 
     * value corresponds to the metadata value.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param StructValue The struct containing metadata values to be set for the string table entry.
     * @return True, if the metadata was successfully set; False, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Set String Table Entry Meta-Data from Stuct", CustomStructureParam = "StructValue", AdvancedDisplay = ReturnValue))
    static bool SetStringTableEntryMetaDataFromStruct(const FName TableId, const FString& Key, const FGenericStruct& StructValue);
    DECLARE_FUNCTION(execSetStringTableEntryMetaDataFromStruct);
    
    /**
     * Sets the metadata with the specified ID for a specific entry in the specified string table.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param MetaDataId The identifier of the metadata to be set.
     * @param MetaDataValue The value of the metadata to be set.
     * @return True, if the metadata was successfully set; False, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Set String Table Entry Meta-Data"))
    static bool SetStringTableEntryMetaData(const FName TableId, const FString& Key, const FName MetaDataId, FString MetaDataValue);

    /**
     * Removes the metadata with specified ID from the specified string table entry.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @param MetaDataId The identifier of the metadata to be removed.
     * @return True, if the metadata was successfully removed; False, if the string table, the entry, or the metadata do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Remove String Table Entry Meta-Data"))
    static bool RemoveStringTableEntryMetaData(const FName TableId, const FString& Key, const FName MetaDataId);

    /**
     * Removes all metadata from the specified string table entry.
     *
     * @param TableId The identifier of the string table.
     * @param Key The key of the string table entry.
     * @return True, if the metadata was successfully removed; False, if the string table, the entry, or the metadata do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Clear Meta-Data from String Table Entry"))
    static bool ClearMetaDataFromStringTableEntry(const FName TableId, const FString& Key);

    /**
     * Removes all metadata from the specified string table.
     *
     * @param TableId The identifier of the string table.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Clear String Table Meta-Data"))
    static void ClearStringTableMetaData(const FName TableId);
#endif
};
