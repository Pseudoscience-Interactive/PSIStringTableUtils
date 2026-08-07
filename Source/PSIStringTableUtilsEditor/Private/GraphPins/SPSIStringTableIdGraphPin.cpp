// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "SPSIStringTableIdGraphPin.h"
#include "PSIStringTableEditorHelper.h"
#include "PSIStringTableHelper.h"
#include "ScopedTransaction.h"
#include "Misc/OutputDeviceNull.h"

#define LOCTEXT_NAMESPACE "PSIStringTableUtils"

void SPSIStringTableIdGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin)
{
    SGraphPin::Construct(SGraphPin::FArguments(), InGraphPin);
}

void SPSIStringTableIdGraphPin::ParseDefaultValueData()
{
    StringTableId.Reset();
    
    const UScriptStruct* Struct = FPSIStringTableId::StaticStruct();
    
    FOutputDeviceNull NullOut;
    Struct->ImportText(*GraphPinObj->GetDefaultAsString(), &StringTableId, nullptr, PPF_SerializedAsImportText,
                       &NullOut, Struct->GetStructCPPName(), true);
}

TSharedRef<SWidget> SPSIStringTableIdGraphPin::GetDefaultValueWidget()
{
    if (GraphPinObj == nullptr)
    {
        return SNullWidget::NullWidget;
    }

    ParseDefaultValueData();

    return SNew(SBox)
        .Visibility(this, &SGraphPin::GetDefaultValueVisibility)
        [
            FPSIStringTableEditorHelper::MakeStringTableSelector(
                FOnGetPropertyComboBoxValue::CreateSP(this, &SPSIStringTableIdGraphPin::OnGetTableIdValue),
                FOnPropertyComboBoxValueSelected::CreateSP(this, &SPSIStringTableIdGraphPin::OnSetTableIdValue))
        ];
}

FString SPSIStringTableIdGraphPin::OnGetTableIdValue() const
{
    return FPSIStringTableHelper::GetStringTableDisplayString(StringTableId);
}

void SPSIStringTableIdGraphPin::OnSetTableIdValue(const FString& NewValue)
{
    FName TableId;
    FPSIStringTableHelper::FindStringTableByName(*NewValue, &TableId);
    StringTableId.Name = TableId;
    
    const FString TableIdString = TableId.ToString();

    const FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
    if (!CurrentDefaultValue.Equals(TableIdString))
    {
        const FScopedTransaction Transaction(LOCTEXT("ChangeDefaultValue", "Change Pin Default Value"));
        GraphPinObj->Modify();
        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, TableIdString);
    }
}

#undef LOCTEXT_NAMESPACE