// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableEditorHelper.h"
#include "Widgets/SCompoundWidget.h"

class IPropertyHandle;

/**
 * Value widget for an 'FDataRegistryType' property.
 * Based on: 'FDataRegistryTypeCustomization'
 */
class SPSIStringTableId : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPSIStringTableId) : _ShowSearchForItemCount(20)
        {
        }

        SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
        SLATE_ARGUMENT(int32, ShowSearchForItemCount)
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
                                                                 InArgs._ShowSearchForItemCount)
        ];
    }
};



