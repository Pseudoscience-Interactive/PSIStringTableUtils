// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableEditorHelper.h"
#include "Widgets/SCompoundWidget.h"

class IPropertyHandle;

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
        //static const FName NAME_TableId(TEXT("TableId"));
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
            StringTablePropertyHandle.ToSharedRef(), bAllowClear, InArgs._ShowSearchForItemCount);

        StringTableKeyWidget = FPSIStringTableEditorHelper::MakeStringTableKeySelector(
            TAttribute<FName>::CreateSP(this, &SPSIStringTableEntryHandle::GetStringTableIdName),
            StringTableKeyPropertyHandle.ToSharedRef(), InArgs._ShowSearchForItemCount);
        
        // Setup Change callback
        const FSimpleDelegate StringTableChangedDelegate = FSimpleDelegate::CreateSP(this, &SPSIStringTableEntryHandle::OnStringTableChanged);
        StructPropertyHandle->SetOnPropertyValueChanged(StringTableChangedDelegate);
        StringTablePropertyHandle->SetOnPropertyValueChanged(StringTableChangedDelegate);
        
        ChildSlot
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                StringTableWidget.ToSharedRef()
            ]

            + SHorizontalBox::Slot()
            [
                StringTableKeyWidget.ToSharedRef()
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

        const FStringTableConstPtr StringTable = FPSIStringTableHelper::FindStringTableByName(TableId);
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
    
    TSharedPtr<IPropertyHandle> StringTablePropertyHandle;
    TSharedPtr<IPropertyHandle> StringTableKeyPropertyHandle;
    TSharedPtr<SWidget> StringTableWidget;
    TSharedPtr<SWidget> StringTableKeyWidget;
};
