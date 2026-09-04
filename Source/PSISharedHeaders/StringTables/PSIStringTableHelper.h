// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"
#include "PSIEngineVersionComparison.h"

/**
 * Helper functions for dealing with String Tables.
 */
namespace FPSIStringTableHelper
{
    /** Whether the specified String Table has any entries. */
    [[nodiscard]] FORCEINLINE bool HasStringTableEntries(const FStringTableConstPtr& StringTable)
    {
        bool bHasEntries = false;
    
        StringTable->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
        {
            bHasEntries = true;
            return false; // stop enumeration
        });
    
        return bHasEntries;
    }

    [[nodiscard]] FORCEINLINE FARCompiledFilter GetStringTableFilter()
    {
        struct FInternals
        {
            static FARCompiledFilter CreateFilter()
            {
                FARFilter Filter;
                Filter.ClassPaths.Add(UStringTable::StaticClass()->GetClassPathName());
                
                FARCompiledFilter CompiledFilter;
                IAssetRegistry::Get()->CompileFilter(Filter, CompiledFilter);
          
                return CompiledFilter;
            }
        };
        
        static FARCompiledFilter StringTableFilter = FInternals::CreateFilter();
        return StringTableFilter;
    }
    
    /** Gets the display string for the specified String Table ID. */
    [[nodiscard]] FORCEINLINE FString GetStringTableDisplayString(const FName& StringTableId)
    {
        const FSoftObjectPath SoftObjectPath = FSoftObjectPath(StringTableId.ToString());
        return SoftObjectPath.IsValid() ? SoftObjectPath.GetAssetName() : StringTableId.ToString();
    }
    
    /** Describes an available String Table with its ID and display name. */
    struct FAvailableStringTable
    {
        FName TableId;
        FText DisplayName;
    };
    
    /**
     * Gets all currently available String Tables with their IDs and display names.
     * @param bIncludeEmptyStringTables Whether String Tables that do not have any entries yet should be included.
     */
    FORCEINLINE void GetAvailableStringTables(TArray<FAvailableStringTable>& OutAvailableStringTables, bool bIncludeEmptyStringTables = false)
    {
        // Based on 'STextPropertyEditableStringTableReference::UpdateStringTableComboOptions'
        //
        
        OutAvailableStringTables.Reset();

        // Process assets first (as they may currently be unloaded)
        const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
        
        AssetRegistryModule.Get().EnumerateAssets(GetStringTableFilter(), [&](const FAssetData& AssetData)
        {
            const FName StringTableId = *AssetData.GetObjectPathString();
            
            // By default, only string table assets that have entries are visible, since selecting an empty one causes
            // unexpected behavior in most pickers. Callers that can handle an empty String Table (e.g. to create its
            // first entry) can opt in via 'bIncludeEmptyStringTables'.
            bool bHasEntries;
            const FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(StringTableId);
            if (StringTable.IsValid())
            {
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
                if (StringTable->IsInternal())
                {
                    // Can't pick internal string tables.
                    // continue
                    return true;
                }
#endif

                bHasEntries = HasStringTableEntries(StringTable);
            }
            else
            {
                // Asset is currently unloaded, so just assume it has entries
                bHasEntries = true;
            }

            if (!bHasEntries && !bIncludeEmptyStringTables)
            {
                // continue
                return true;
            }

            FAvailableStringTable& AvailableStringTableEntry = OutAvailableStringTables.AddDefaulted_GetRef();
            AvailableStringTableEntry.TableId = StringTableId;
            AvailableStringTableEntry.DisplayName = FText::FromName(AssetData.AssetName);
            
            // continue
            return true;
        });
        
        // Process the remaining non-asset string tables now
        FStringTableRegistry::Get().EnumerateStringTables([&](const FName& InTableId, const FStringTableConstRef& StringTable) -> bool
        {
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
            if (StringTable->IsInternal())
            {
                // Can't pick internal string tables.
                //
                // continue 
                return true;
            }
#endif

            const bool bAlreadyAdded = OutAvailableStringTables.ContainsByPredicate([InTableId](const FAvailableStringTable& InAvailableStringTable)
            {
                return InAvailableStringTable.TableId == InTableId;
            });
            
            if (!bAlreadyAdded && (bIncludeEmptyStringTables || HasStringTableEntries(StringTable)))
            {
                FAvailableStringTable& AvailableStringTableEntry = OutAvailableStringTables.AddDefaulted_GetRef();
                AvailableStringTableEntry.TableId = InTableId;
                AvailableStringTableEntry.DisplayName = FText::FromName(InTableId);
            }

            // continue
            return true;
        });

        OutAvailableStringTables.Sort([](const FAvailableStringTable& InOne, const FAvailableStringTable& InTwo)
        {
            return InOne.DisplayName.ToString() < InTwo.DisplayName.ToString();
        });
    }
    
    /**
     * Finds the String Table with the specified name, if it exists.
     * @param bIncludeEmptyStringTables Whether a String Table that does not have any entries yet should be found.
     */
    FORCEINLINE FStringTableConstPtr FindStringTableByName(const FName& StringTableName, FName* OutStringTableId = nullptr, bool bIncludeEmptyStringTables = false)
    {
        if (StringTableName.IsNone())
        {
            return nullptr;
        }
        
        // Process assets first (as they may currently be unloaded)
        //
        const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

        FStringTableConstPtr Result;

        AssetRegistryModule.Get().EnumerateAssets(GetStringTableFilter(), [&](const FAssetData& AssetData)
        {
            const FName StringTableId = *AssetData.GetObjectPathString();
            
            // By default, only string table assets that have entries are visible, since selecting an empty one causes
            // unexpected behavior in most pickers. Callers that can handle an empty String Table (e.g. to create its
            // first entry) can opt in via 'bIncludeEmptyStringTables'.
            bool bHasEntries;
            FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(StringTableId);
            if (StringTable.IsValid())
            {
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
                if (StringTable->IsInternal())
                {
                    // Can't pick internal string tables.
                    //
                    // continue 
                    return true;
                }
#endif

                bHasEntries = HasStringTableEntries(StringTable);
            }
            
            else
            {
                // Asset is currently unloaded, so just assume it has entries
                bHasEntries = true;
            }

            // We make sure that we check for the ID itself, for a simple asset name as well as for more complex object
            // paths. The latter ensures that we can assign asset references copied from the content browser.
            if ((bHasEntries || bIncludeEmptyStringTables) && (
                StringTableName == StringTableId ||
                StringTableName == AssetData.AssetName ||
                FSoftObjectPath(StringTableName.ToString()) == AssetData.ToSoftObjectPath()))
            {
                if (!StringTable.IsValid())
                {
                    // Force load the asset.
                    // ReSharper disable once CppExpressionWithoutSideEffects
                    AssetData.GetAsset();
                    
                    // Now that it is loaded, it should also be registered.
                    StringTable = FStringTableRegistry::Get().FindStringTable(StringTableId);
                }
                
                Result = StringTable;
                if (OutStringTableId != nullptr)
                {
                    *OutStringTableId = StringTableId;
                }
                
                // break
                return false;
            }
            
            // continue 
            return true;
        });

        if (Result.IsValid())
        {
            return Result;
        }

        // Process the remaining non-asset string tables now
        FStringTableRegistry::Get().EnumerateStringTables([&](const FName& StringTableId,
                                                              const FStringTableConstRef& StringTable) -> bool
        {
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 5, 0)
            if (StringTable->IsInternal())
            {
                // Can't pick internal string tables.
                //
                // continue 
                return true;
            }
#endif

            if ((bIncludeEmptyStringTables || HasStringTableEntries(StringTable)) && StringTableName == StringTableId)
            {
                Result = StringTable;
                if (OutStringTableId != nullptr)
                {
                    *OutStringTableId = StringTableId;
                }
                
                // break
                return false;
            }

            // continue
            return true;
        });
        
        return Result;
    }
}