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
    [
        SNew(SPSIStringTableId)
        .StructPropertyHandle(StructPropertyHandle)
        .IncludeEmptyStringTables(true)
    ];
}

void FPSIStringTableIdTypeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}
