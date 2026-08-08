// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableEntryHandleTypeCustomization.h"
#include "PSIStringTableEditorHelper.h"

void FPSIStringTableEntryHandleTypeCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                                  FDetailWidgetRow& HeaderRow,
                                                                  IPropertyTypeCustomizationUtils&
                                                                  StructCustomizationUtils)
{
    HeaderRow
    .NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ];
    HeaderRow
    .ValueContent()
    .MinDesiredWidth(500.f)
    [
        SNew(SPSIStringTableEntryHandle)
        .StructPropertyHandle(StructPropertyHandle)
    ];
}

void FPSIStringTableEntryHandleTypeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

