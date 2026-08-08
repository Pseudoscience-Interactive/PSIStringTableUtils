// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableIdTypeCustomization.h"
#include "PSIStringTableEditorHelper.h"

void FPSIStringTableIdTypeCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                         FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    HeaderRow
    .NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ];
    HeaderRow
    .ValueContent()
    .MinDesiredWidth(250.f)
    [
        SNew(SPSIStringTableId)
        .StructPropertyHandle(StructPropertyHandle)
    ];
}

void FPSIStringTableIdTypeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}
