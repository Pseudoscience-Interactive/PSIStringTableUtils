// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableHelper.h"
#include "Internationalization/StringTableCore.h"
#include "PSIEngineVersionComparison.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Widgets/SToolTip.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWrapBox.h"

/**
 * Helper functions for dealing with String Tables in the editor.
 */
class FPSIStringTableEditorHelper
{
    FORCEINLINE static void OnGetStringTableStrings(TArray<TSharedPtr<FString>>& OutStrings,
        TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems, bool bAllowClear,
        bool bIncludeEmptyStringTables)
    {
        TArray<FPSIStringTableHelper::FAvailableStringTable> AvailableStringTables;
        FPSIStringTableHelper::GetAvailableStringTables(AvailableStringTables, bIncludeEmptyStringTables);

        if (bAllowClear)
        {
            // Add None
            OutStrings.Add(MakeShared<FString>(NSLOCTEXT("PSIStringTableEditorHelper", "NoType", "None").ToString()));
            OutToolTips.Add(SNew(SToolTip).Text(NSLOCTEXT("PSIStringTableEditorHelper", "NoType", "None")));
            OutRestrictedItems.Add(false);
        }
        
        for (const FPSIStringTableHelper::FAvailableStringTable& AvailableStringTable : AvailableStringTables)
        {
            OutStrings.Add(MakeShared<FString>(AvailableStringTable.DisplayName.ToString()));
            OutToolTips.Add(SNew(SToolTip).Text(FText::FromName(AvailableStringTable.TableId)));
            OutRestrictedItems.Add(false);
        }
    }

    FORCEINLINE static FString OnGetStringTableValue(TSharedRef<IPropertyHandle> StringTableHandle)
    {
        FName StringTableId;
        StringTableHandle->GetValue(StringTableId);
        
        FString StringTableName = StringTableId.ToString();
        if (StringTableId.IsNone())
        {
            return StringTableName;
        }

        const FSoftObjectPath ObjectPath(StringTableName);
        if (ObjectPath.IsValid())
        {
            return ObjectPath.GetAssetName();
        }
        
        return StringTableName;
    }
    
    FORCEINLINE static void OnStringTableSelected(const FString& String,
        TSharedRef<IPropertyHandle> PropertyHandle, bool bIncludeEmptyStringTables)
    {
        FName StringTableId;
        FPSIStringTableHelper::FindStringTableByName(FName(String), &StringTableId, bIncludeEmptyStringTables);
        PropertyHandle->SetValue(StringTableId);
    }

    FORCEINLINE static void OnGetStringTableKeyStrings(
        TArray<TSharedPtr<FString>>& OutStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips,
        TArray<bool>& OutRestrictedItems, TAttribute<FName> StringTableId)
    {
        const FStringTableConstPtr StringTable = FPSIStringTableHelper::FindStringTableByName(StringTableId.Get());
        if (!StringTable.IsValid())
        {
            return;
        }
        
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
        // Dev Notes were introduced in UE 5.8  
        StringTable->EnumerateKeysAndSourceStrings([&](const FTextKey& Key, const FString& SourceString, const FString& DevNotes)
        {
            OutStrings.Emplace(MakeShared<FString>(Key.ToString()));
            OutToolTips.Add(SNew(SToolTip).Text(FText::FromString(DevNotes)));
            OutRestrictedItems.Add(false);
                
            // continue
            return true;
        });
#else
        StringTable->EnumerateKeysAndSourceStrings([&](const FTextKey& Key, const FString& SourceString)
        {
            OutStrings.Emplace(MakeShared<FString>(FPSIStringTableHelper::GetTextKeyString(Key)));
            OutRestrictedItems.Add(false);
            
            // continue
            return true;
        });
#endif
    }
    
public:
    /** 
     * Gets the display text for the value of 'IPropertyHandle' that points to an 'FName' or an 'FPSIStringTableId'.
     */
    [[nodiscard]] FORCEINLINE static FText OnGetStringTableValueText(TSharedRef<IPropertyHandle> PropertyHandle)
    {
        return FText::FromString(OnGetStringTableValue(PropertyHandle));
    }

    /** 
     * Creates a combo box widget for selecting a String Table. 
     * @param OnGetDisplayString Callback to get the display string for the currently selected String Table.
     * @param OnSetValue Callback to set the selected String Table.
     * @param bAllowClear Whether to allow clearing the selection.
     * @param ShowSearchForItemCount If the number of items in the combo box is greater than this value, it will show a
     * search box to allow filtering. -1 means to never show it.
     * @param bIncludeEmptyStringTables Whether String Tables that do not have any entries yet should be selectable.
     */
    [[nodiscard]] FORCEINLINE static TSharedRef<SWidget> MakeStringTableSelector(
        const FOnGetPropertyComboBoxValue& OnGetDisplayString,
        const FOnPropertyComboBoxValueSelected& OnSetValue,
        bool bAllowClear = true,
        int32 ShowSearchForItemCount = 20,
        bool bIncludeEmptyStringTables = false)
    {
        FPropertyComboBoxArgs ComboArgs;
        ComboArgs.ShowSearchForItemCount = ShowSearchForItemCount;
        ComboArgs.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateStatic(&OnGetStringTableStrings, bAllowClear, bIncludeEmptyStringTables);
        ComboArgs.OnGetValue = OnGetDisplayString;
        ComboArgs.OnValueSelected = OnSetValue;

        return PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs);
    }

    /** 
     * Creates a combo box widget for selecting a String Table. 
     * @param PropertyHandle The 'IPropertyHandle' that points to an 'FName' or an 'FPSIStringTableId' property.
     * ID.
     * @param bAllowClear Whether to allow clearing the selection.
     * @param ShowSearchForItemCount If the number of items in the combo box is greater than this value, it will show a
     * search box to allow filtering. -1 means to never show it.
     * @param bIncludeEmptyStringTables Whether String Tables that do not have any entries yet should be selectable.
     */
    [[nodiscard]] FORCEINLINE static TSharedRef<SWidget> MakeStringTableSelector(
        const TSharedRef<IPropertyHandle>& PropertyHandle,
        bool bAllowClear = true,
        int32 ShowSearchForItemCount = 20,
        bool bIncludeEmptyStringTables = false)
    {
        FPropertyComboBoxArgs ComboArgs;
        ComboArgs.PropertyHandle = PropertyHandle;
        ComboArgs.ShowSearchForItemCount = ShowSearchForItemCount;
        ComboArgs.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateStatic(&OnGetStringTableStrings, bAllowClear, bIncludeEmptyStringTables);
        ComboArgs.OnGetValue = FOnGetPropertyComboBoxValue::CreateStatic(&OnGetStringTableValue, PropertyHandle);
        ComboArgs.OnValueSelected = FOnPropertyComboBoxValueSelected::CreateStatic(&OnStringTableSelected, PropertyHandle, bIncludeEmptyStringTables);

        return PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs);
    }
    
    /** 
     * Creates a combo box widget for selecting a key of a specific String Table.
     * @param StringTableId The ID of the String Table to select a key from.
     * @param OnGetDisplayString Callback to get the display string for the currently selected String Table key.
     * @param OnSetValue Callback to set the selected String Table key.
     * @param ShowSearchForItemCount If the number of items in the combo box is greater than this value, it will show a 
     * search box to allow filtering. -1 means to never show it.
     */
    [[nodiscard]] FORCEINLINE static TSharedRef<SWidget> MakeStringTableKeySelector(
        const TAttribute<FName>& StringTableId,
        const FOnGetPropertyComboBoxValue& OnGetDisplayString,
        const FOnPropertyComboBoxValueSelected& OnSetValue,
        int32 ShowSearchForItemCount = 20)
    {
        FPropertyComboBoxArgs ComboArgs;
        ComboArgs.ShowSearchForItemCount = ShowSearchForItemCount;
        ComboArgs.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateStatic(&OnGetStringTableKeyStrings, StringTableId);
        ComboArgs.OnGetValue = OnGetDisplayString;
        ComboArgs.OnValueSelected = OnSetValue;
        
        return PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs);
    }
    
    /** 
     * Creates a combo box widget for selecting a key of a specific String Table.
     * @param StringTableId The ID of the String Table to select a key from.
     * @param PropertyHandle The 'IPropertyHandle' that points to an 'FString' property that represents the String Table 
     * key.
     * @param ShowSearchForItemCount If the number of items in the combo box is greater than this value, it will show a 
     * search box to allow filtering. -1 means to never show it.
     */
    [[nodiscard]] FORCEINLINE TSharedRef<SWidget> static MakeStringTableKeySelector(
        const TAttribute<FName>& StringTableId,
        const TSharedRef<IPropertyHandle>& PropertyHandle,
        int32 ShowSearchForItemCount = 20)
    {
        FPropertyComboBoxArgs ComboArgs;
        ComboArgs.PropertyHandle = PropertyHandle;
        ComboArgs.ShowSearchForItemCount = ShowSearchForItemCount;
        ComboArgs.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateStatic(&OnGetStringTableKeyStrings, StringTableId);
        
        return PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs);
    }
};

class IPropertyHandle;

/**
 * Value widget for an 'FDataRegistryType' property.
 * Based on: 'FDataRegistryTypeCustomization'
 */
class SPSIStringTableId : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPSIStringTableId) : _ShowSearchForItemCount(20), _IncludeEmptyStringTables(false)
    {
    }

        SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
        SLATE_ARGUMENT(int32, ShowSearchForItemCount)

        /** Whether String Tables that do not have any entries yet should be selectable. */
        SLATE_ARGUMENT(bool, IncludeEmptyStringTables)
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs)
    {
        const TSharedRef<IPropertyHandle> StructPropertyHandle = InArgs._StructPropertyHandle.ToSharedRef();
        const bool bAllowClear = !(StructPropertyHandle->GetMetaDataProperty()->PropertyFlags & CPF_NoClear);

        const TSharedRef<IPropertyHandle> NamePropertyHandle = StructPropertyHandle->GetChildHandle(NAME_Name).ToSharedRef();
        
        ChildSlot
        [
            FPSIStringTableEditorHelper::MakeStringTableSelector(NamePropertyHandle, bAllowClear,
                                                                 InArgs._ShowSearchForItemCount,
                                                                 InArgs._IncludeEmptyStringTables)
        ];
    }
};

/**
 * Value widget for an 'FPSIStringTableEntryHandle' property.
 */
class SPSIStringTableEntryHandle : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPSIStringTableEntryHandle)  : _ShowSearchForItemCount(20)
        {
        }

        SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
        SLATE_ARGUMENT(int32, ShowSearchForItemCount)
    SLATE_END_ARGS()
    
    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs)
    {
        static const FName NAME_Key(TEXT("Key"));
        
        const TSharedRef<IPropertyHandle> StructPropertyHandle = InArgs._StructPropertyHandle.ToSharedRef();
        StringTablePropertyHandle = StructPropertyHandle->GetChildHandle(NAME_Name);
        if (!StringTablePropertyHandle.IsValid())
        {
            return;
        }
        
        StringTableKeyPropertyHandle = StructPropertyHandle->GetChildHandle(NAME_Key);
        if (!StringTableKeyPropertyHandle.IsValid())
        {
            return;
        }
        
        const bool bAllowClear = !(StructPropertyHandle->GetMetaDataProperty()->PropertyFlags & CPF_NoClear);

        StringTableWidget = FPSIStringTableEditorHelper::MakeStringTableSelector(
            StringTablePropertyHandle.ToSharedRef(), bAllowClear, InArgs._ShowSearchForItemCount, /*bIncludeEmptyStringTables*/ true);

        StringTableKeyWidget = FPSIStringTableEditorHelper::MakeStringTableKeySelector(
            TAttribute<FName>::CreateSP(this, &SPSIStringTableEntryHandle::GetStringTableIdName),
            StringTableKeyPropertyHandle.ToSharedRef(), InArgs._ShowSearchForItemCount);
        
        // Setup Change callback
        const FSimpleDelegate StringTableChangedDelegate = FSimpleDelegate::CreateSP(this, &SPSIStringTableEntryHandle::OnStringTableChanged);
        StructPropertyHandle->SetOnPropertyValueChanged(StringTableChangedDelegate);
        StringTablePropertyHandle->SetOnPropertyValueChanged(StringTableChangedDelegate);
        
        ChildSlot
        [
            SNew(SWrapBox)
            .UseAllottedSize(true)
            .Orientation(Orient_Horizontal)
            .HAlign(HAlign_Fill)
            + SWrapBox::Slot()
            [
                StringTableWidget.ToSharedRef()
            ]

            + SWrapBox::Slot()
            [
                SNew(SBox)
                .Visibility(this, &SPSIStringTableEntryHandle::GetKeySelectorVisibility)
                [
                    StringTableKeyWidget.ToSharedRef()
                ]
            ]
        ];
    }
    
    /** Gets the underlying value widget for the 'TableId' property. */
    TSharedPtr<SWidget> GetStringTableIdWidget() const
    {
        return StringTableWidget;
    }
    
    /** Gets the underlying value widget for the 'Key' property. */
    TSharedPtr<SWidget> GetStringTableKeyWidget() const
    {
        return StringTableKeyWidget;
    }
    
    TSharedPtr<IPropertyHandle> GetStringTableIdPropertyHandle() const
    {
        return StringTablePropertyHandle;
    }
    
    TSharedPtr<IPropertyHandle> GetStringTableKeyPropertyHandle() const
    {
        return StringTableKeyPropertyHandle;
    }
    
    /** Open the reference viewer */
    void OnSearchForReferences();
    
private:
    void OnStringTableChanged() const
    {        
        FName TableId;
        StringTablePropertyHandle->GetValue(TableId);
        
        FString StringTableKey;
        StringTableKeyPropertyHandle->GetValue(StringTableKey);

        const FStringTableConstPtr StringTable = FPSIStringTableHelper::FindStringTableByName(TableId, nullptr, /*bIncludeEmptyStringTables*/ true);
        if (!StringTable.IsValid())
        {
            StringTableKeyPropertyHandle->SetValue(FString());
        }
        
        else if (!StringTable->FindEntry(StringTableKey))
        {
            StringTableKey.Reset();
            StringTable->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
            {
                StringTableKey = InKey;
                return false; // stop enumeration
            });
            
            StringTableKeyPropertyHandle->SetValue(StringTableKey);
        }
    }
    
    FName GetStringTableIdName() const
    {
        FName StringTableId;
        StringTablePropertyHandle->GetValue(StringTableId);
        return StringTableId;
    }
    
    EVisibility GetKeySelectorVisibility() const
    {
        return GetStringTableIdName().IsNone() ? EVisibility::Collapsed : EVisibility::Visible;
    }
    
    TSharedPtr<IPropertyHandle> StringTablePropertyHandle;
    TSharedPtr<IPropertyHandle> StringTableKeyPropertyHandle;
    TSharedPtr<SWidget> StringTableWidget;
    TSharedPtr<SWidget> StringTableKeyWidget;
};