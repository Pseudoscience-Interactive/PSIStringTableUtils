// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIRequires.h"
#include "PSITypeTraits.h"

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
    bool DoesStringTableEntryHasMetaData(const FStringTableConstPtr& StringTable, const FTextKey& Key);
    
#if WITH_EDITOR
    /** 
     * Notifies that the 'OwnerAsset' of the specified string table has been changed. Does nothing if the string table 
     * does not have an owner asset. Should be called everytime the string table is modified.
     */
    void PSISTRINGTABLEUTILS_API NotifyStringTableChanged(const FStringTableConstPtr& StringTable);
#endif
};
