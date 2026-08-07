// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

/**
 * Property type customization for the 'FPSIStringTableId' struct.
 */
class FPSIStringTableIdTypeCustomization final : public IPropertyTypeCustomization
{
public:
    FPSIStringTableIdTypeCustomization() {}

    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShared<FPSIStringTableIdTypeCustomization>();
    }
    
    //~ Begin IPropertyTypeCustomization interface
    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
                                 IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                   IDetailChildrenBuilder& StructBuilder,
                                   IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
    //~ End IPropertyTypeCustomization interface
};