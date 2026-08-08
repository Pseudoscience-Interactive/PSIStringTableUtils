// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableId.h"
#include "PSIStringTableEntryHandle.h"
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

    /** Gets the ID of the specified string table asset. */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities", meta=(DisplayName="Get String Table ID"))
    static FPSIStringTableId GetStringTableId(const UStringTable* StringTable);
    
    /** Returns true if the given table ID corresponds to a registered string table. */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities")
    static bool IsStringTableRegistered(FPSIStringTableId TableId);

    /** Returns true if the given table ID corresponds to a registered string table, and that table has. */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities")
    static bool IsStringTableEntryRegistered(FPSIStringTableEntryHandle Entry);

    /**
     * Gets the namespace of the string table with the specified ID.
     *
     * @param TableId The ID of the string table.
     * @return The namespace of the string table.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", Blueprintpure)
    static FString GetStringTableNamespace(FPSIStringTableId TableId);

    /**
     * Gets the source string of a specific string table entry. 
     *
     * @param Entry Handle of the string table entry.
     * @return The source string, if found or an empty string otherwise.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintPure)
    static FString GetStringTableSourceString(FPSIStringTableEntryHandle Entry);

    /** Returns the specified meta-data of the given string table entry (or an empty string). */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities", meta=(DisplayName="Get String Table Entry Meta-Data"))
    static FString GetStringTableEntryMetaData(FPSIStringTableEntryHandle Entry, const FName MetaDataId);

    /** Returns an array of all keys within the given string table */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities")
    static TArray<FString> GetKeysFromStringTable(FPSIStringTableId TableId);

    /** Returns an array of all meta-data IDs within the given string table entry */
    UFUNCTION(BlueprintPure, Category = "PSI/String Table Utilities", meta=(DisplayName="Get Meta-Data IDs from String Table Entry"))
    static TArray<FName> GetMetaDataIdsFromStringTableEntry(FPSIStringTableEntryHandle Entry);

    /** Export the key, string, and meta-data information in the given string table to a CSV string (does not export the namespace) */
    UFUNCTION(BlueprintCallable, Category = "PSI/String Table Utilities", meta=(DisplayName="Export String Table to CSV String"))
    static bool ExportStringTableToCSVString(FPSIStringTableId TableId, FString& OutCSVString);

    /** Export the key, string, and meta-data information in the given string table to a CSV file (does not export the namespace) */
    UFUNCTION(BlueprintCallable, Category = "PSI/String Table Utilities", meta=(DisplayName="Export String Table to CSV File"))
    static bool ExportStringTableToCSVFile(FPSIStringTableId TableId, const FString& Filename);

    /** Import key, string, and meta-data information from a CSV string to the given string table (does not import the namespace) */
    UFUNCTION(BlueprintCallable, Category = "PSI/String Table Utilities", meta=(DisplayName="Import String Table from CSV String"))
    static bool ImportStringTableFromCSVString(FPSIStringTableId TableId, const FString& CSVString);

    /** Import key, string, and meta-data information from a CSV file to the given string table (does not import the namespace) */
    UFUNCTION(BlueprintCallable, Category = "PSI/String Table Utilities", meta=(DisplayName="Import String Table from CSV File"))
    static bool ImportStringTableFromCSVFile(FPSIStringTableId TableId, const FString& Filename);
    
    /**
     * Gets the metadata of a specific string table entry as property values of a given struct. Each property of the 
     * struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if the 
     * authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @param Entry Handle of the string table entry.
     * @param StructValue The output struct to store the metadata values.
     * @param RetrievedValues The total number of metadata values retrieved.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Get String Table Entry Meta-Data as Stuct", CustomStructureParam = "StructValue", AdvancedDisplay = RetrievedValues))
    static void GetStringTableEntryMetaDataAsStruct(FPSIStringTableEntryHandle Entry, FGenericStruct& StructValue, int32& RetrievedValues);
    DECLARE_FUNCTION(execGetStringTableEntryMetaDataAsStruct);

    /**
     * Retrieves the metadata of a specific string table entry as property values of a given struct. Each property of 
     * the struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if 
     * the authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @param Entry Handle of the string table entry.
     * @param StructValueRef The output struct to store the metadata values.
     * @param RetrievedValues The total number of metadata values retrieved.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Populate Struct with String Table Entry Meta-Data", CustomStructureParam = "StructValueRef", AdvancedDisplay = RetrievedValues))
    static void PopulateStructWithStringTableEntryMetaData(FPSIStringTableEntryHandle Entry, UPARAM(ref) FGenericStruct& StructValueRef, int32& RetrievedValues);
    DECLARE_FUNCTION(execPopulateStructWithStringTableEntryMetaData);
    
#if WITH_EDITOR
    /**
     * Sets or adds the source string for a specific string table entry. 
     *
     * @param Entry Handle of the string table entry.
     * @param InSourceString The source string to be set for the string table entry.
     * @param bPreserveData Whether any existing data for that entry should be preserved.
     * @return True, if the source string was successfully set; false, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Set String Table Source String", AdvancedDisplay = bPreserveData))
    static bool SetStringTableSourceString(FPSIStringTableEntryHandle Entry, const FString& InSourceString, bool bPreserveData);

    /**
     * Removes a specific string table entry from its string table. This also includes all metadata associated with 
     * that entry.
     *
     * @param Entry Handle of the string table entry.
     * @return True, if the entry was successfully removed; false, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Remove String Table Entry"))
    static bool RemoveStringTableEntry(FPSIStringTableEntryHandle Entry);
    
    /** 
     * Sets the metadata of a specific string table entry from the property values of a given struct. Each property 
     * of the struct is mapped to a metadata, where the property name corresponds to the metadata ID, and the property 
     * value corresponds to the metadata value.
     *
     * @param Entry Handle of the string table entry.
     * @param StructValue The struct containing metadata values to be set for the string table entry.
     * @return True, if the metadata was successfully set; false, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, CustomThunk, meta = (DisplayName="Set String Table Entry Meta-Data from Stuct", CustomStructureParam = "StructValue", AdvancedDisplay = ReturnValue))
    static bool SetStringTableEntryMetaDataFromStruct(FPSIStringTableEntryHandle Entry, const FGenericStruct& StructValue);
    DECLARE_FUNCTION(execSetStringTableEntryMetaDataFromStruct);
    
    /**
     * Sets the metadata with the specified ID for a specific entry in the specified string table.
     *
     * @param Entry Handle of the string table entry.
     * @param MetaDataId The identifier of the metadata to be set.
     * @param MetaDataValue The value of the metadata to be set.
     * @return True, if the metadata was successfully set; false, if the string table or the entry do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Set String Table Entry Meta-Data"))
    static bool SetStringTableEntryMetaData(FPSIStringTableEntryHandle Entry, const FName MetaDataId, FString MetaDataValue);

    /**
     * Removes the metadata with specified ID from the specified string table entry.
     *
     * @param Entry Handle of the string table entry.
     * @param MetaDataId The identifier of the metadata to be removed.
     * @return True, if the metadata was successfully removed; false, if the string table, the entry, or the metadata do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Remove String Table Entry Meta-Data"))
    static bool RemoveStringTableEntryMetaData(FPSIStringTableEntryHandle Entry, const FName MetaDataId);

    /**
     * Removes all metadata from the specified string table entry.
     *
     * @param Entry Handle of the string table entry.
     * @return True, if the metadata was successfully removed; false, if the string table, the entry, or the metadata do not exist.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Clear Meta-Data from String Table Entry"))
    static bool ClearMetaDataFromStringTableEntry(FPSIStringTableEntryHandle Entry);

    /**
     * Removes all metadata from the specified string table.
     *
     * @param TableId The ID of the string table.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable, meta=(DisplayName="Clear String Table Meta-Data"))
    static void ClearStringTableMetaData(FPSIStringTableId TableId);
    
    /**
     * Sets the namespace of the string table with the specified ID.
     *
     * @param TableId The ID of the string table.
     * @param Namespace The namespace of the string table.
     */
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintCallable)
    static void SetStringTableNamespace(FPSIStringTableId TableId, const FString& Namespace);
    
#endif
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // FPSIStringTableId
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // FPSIStringTableEntryHandle
    
    UFUNCTION(Category = "PSI/String Table Utilities", BlueprintPure)
    static FPSIStringTableEntryHandle GetStringTableEntryHandleFromText(const FText& Text);
};
