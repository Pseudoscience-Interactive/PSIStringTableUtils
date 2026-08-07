// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class SPSIStringTableEntryHandle;

/**
 * Property type customization for the 'FPSIStringTableEntryHandle' struct.
 */
class FPSIStringTableEntryHandleTypeCustomization final : public IPropertyTypeCustomization
{
public:
    FPSIStringTableEntryHandleTypeCustomization() {}

    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShared<FPSIStringTableEntryHandleTypeCustomization>();
    }
    
    //~ Begin IPropertyTypeCustomization interface
    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
                                 IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                   IDetailChildrenBuilder& StructBuilder,
                                   IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
    //~ End IPropertyTypeCustomization interface
};
