// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIRequires.h"
#include "PSIIsScriptStruct.h"

/**
 * Helper functions to modify String Tables and to make it easier to retrieve their metadata.
 */
namespace FPSIStringTableUtilsHelper
{
    /**
     * Gets the metadata of the specified string table entry as property values of a given struct. Each property of 
     * the struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if 
     * the authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @param StringTable The string table.
     * @param TextKey The key of the string table entry.
     * @param Struct The type of the struct.
     * @param StructValuePtr A pointer of a value of the struct to store the metadata values.
     * @return The total number of metadata values retrieved.
     */
    int32 PSISTRINGTABLEUTILS_API GetStringTableEntryMetaDataAsStruct(const FStringTableConstPtr& StringTable, const FTextKey& TextKey, const UScriptStruct* Struct, void* StructValuePtr);
    
    /**
     * Gets the metadata of the specified string table entry as property values of a given struct. Each property of 
     * the struct corresponds with a metadata value. A metadata value is parsed and assigned to a struct's property if 
     * the authored name of the property matches the metadata ID.
     * 
     * Note that the struct's properties may retrieve only a subset of the available metadata values. It is 
     * possible to call this function using different structs, each targeting a different set of metadata IDs.
     *
     * @tparam T The type of the struct.
     * @param StringTable The string table.
     * @param TextKey The key of the string table entry.
     * @param StructValue A reference of a value of the struct to store the metadata values.
     * @return The total number of metadata values retrieved.
     */
    template<typename T PSI_REQUIRES(TPSIIsScriptStruct<T>::Value)>
    int32 GetStringTableEntryMetaDataAsStruct(const FStringTableConstPtr& StringTable, const FTextKey& TextKey, T& StructValue)
    {
        return GetStringTableEntryMetaDataAsStruct(StringTable, TextKey, T::StaticStruct(), &StructValue);
    }
    
    /**
     * Sets the metadata of the specified string table entry from the property values of a given struct. Each property 
     * of the  struct is mapped to a metadata, where the property name corresponds to the metadata ID, and the property 
     * value corresponds to the metadata value.
     *
     * @param StringTable The string table.
     * @param TextKey The key of the string table entry.
     * @param Struct The type of the struct.
     * @param StructValuePtr A pointer of a value of the struct containing the metadata values to be set for the string 
     * table entry
     * @return True, if the metadata was successfully set; False, if the string table or the entry do not exist.
     */
    bool PSISTRINGTABLEUTILS_API SetStringTableEntryMetaDataFromStruct(const FStringTablePtr& StringTable, const FTextKey& TextKey, const UScriptStruct* Struct, const void* StructValuePtr);
    
    /**
     * Sets the metadata of the specified string table entry from the property values of a given struct. Each property 
     * of the  struct is mapped to a metadata, where the property name corresponds to the metadata ID, and the property 
     * value corresponds to the metadata value.
     *
     * @tparam T The type of the struct.
     * @param StringTable The string table.
     * @param TextKey The key of the string table entry.
     * @param StructValue The struct containing metadata values to be set for the string table entry.
     * table entry
     * @return True, if the metadata was successfully set; False, if the string table or the entry do not exist.
     */
    template<typename T PSI_REQUIRES(TPSIIsScriptStruct<T>::Value)>
    bool SetStringTableEntryMetaDataFromStruct(const FStringTablePtr& StringTable, const FTextKey& TextKey, const T& StructValue)
    {
        return SetStringTableEntryMetaDataFromStruct(StringTable, TextKey, T::StaticStruct(), &StructValue);
    }
    
    /** Checks whether any metadata is associated with the specified string table entry. */
    bool PSISTRINGTABLEUTILS_API DoesStringTableEntryHasMetaData(const FStringTableConstPtr& StringTable, const FTextKey& Key);

    /**
     * Exports the keys, source strings, and metadata of the specified string table to a CSV string. Works on every
     * supported engine version, even though the engine only provides this function since UE 5.8.
     *
     * @param StringTable The string table.
     * @return The CSV string. Empty if the string table is invalid.
     */
    FString PSISTRINGTABLEUTILS_API ExportStringTableToCSVString(const FStringTableConstPtr& StringTable);

    /**
     * Exports the keys, source strings, and metadata of the specified string table to a CSV file.
     *
     * @param StringTable The string table.
     * @param FilePath The path of the CSV file to write.
     * @return True, if the file was written; false, otherwise.
     */
    bool PSISTRINGTABLEUTILS_API ExportStringTableToCSVFile(const FStringTableConstPtr& StringTable, const FString& FilePath);

    /**
     * Replaces the entries of the specified string table with the ones in a CSV string. Works on every supported
     * engine version, even though the engine only provides this function since UE 5.8.
     *
     * @param StringTable The string table.
     * @param CSVString The CSV string with at least a "Key" and a "SourceString" column.
     * @param ErrorContext The name used to identify the CSV string in warnings.
     * @return True, if the CSV string was imported; false, otherwise.
     */
    bool PSISTRINGTABLEUTILS_API ImportStringTableFromCSVString(const FStringTablePtr& StringTable, const FString& CSVString, const TCHAR* ErrorContext);

    /**
     * Replaces the entries of the specified string table with the ones in a CSV file.
     *
     * @param StringTable The string table.
     * @param FilePath The path of the CSV file to read.
     * @return True, if the file was read and imported; false, otherwise.
     */
    bool PSISTRINGTABLEUTILS_API ImportStringTableFromCSVFile(const FStringTablePtr& StringTable, const FString& FilePath);

#if WITH_EDITOR
    /**
     * Calls 'Modify' on the 'OwnerAsset' of the specified string table, so an open transaction records its state
     * before the change. Must be called before every change of the string table. Does nothing if the string table
     * does not have an owner asset.
     */
    void PSISTRINGTABLEUTILS_API PreStringTableChange(const FStringTableConstPtr& StringTable);

    /**
     * Notifies that the 'OwnerAsset' of the specified string table has been changed. Does nothing if the string table
     * does not have an owner asset. Should be called everytime the string table is modified, after
     * 'PreStringTableChange' was called before the change.
     */
    void PSISTRINGTABLEUTILS_API NotifyStringTableChanged(const FStringTableConstPtr& StringTable);
#endif
};
