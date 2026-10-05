// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "SPSIStringTableEntryHandleGraphPin.h"
#include "PropertyCustomizationHelpers.h"
#include "PSIStringTableEditorHelper.h"
#include "PSIStringTableHelper.h"
#include "ScopedTransaction.h"
#include "Misc/OutputDeviceNull.h"

#define LOCTEXT_NAMESPACE "PSIStringTableUtils"

void SPSIStringTableEntryHandleGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin)
{
    SGraphPin::Construct(SGraphPin::FArguments(), InGraphPin);
}

void SPSIStringTableEntryHandleGraphPin::ParseDefaultValueData()
{
    StringTableEntryHandle.Reset();

    UScriptStruct* Struct = FPSIStringTableEntryHandle::StaticStruct();
    
    FOutputDeviceNull NullOut;
    Struct->ImportText(*GraphPinObj->GetDefaultAsString(), &StringTableEntryHandle, nullptr, PPF_SerializedAsImportText,
                       &NullOut, Struct->GetStructCPPName(), true);
}

TSharedRef<SWidget> SPSIStringTableEntryHandleGraphPin::GetDefaultValueWidget()
{
    if (GraphPinObj == nullptr)
    {
        return SNullWidget::NullWidget;
    }

    ParseDefaultValueData();

    return SNew(SWrapBox)
        .Visibility(this, &SGraphPin::GetDefaultValueVisibility)
        + SWrapBox::Slot()
        [
            FPSIStringTableEditorHelper::MakeStringTableSelector(
                FOnGetPropertyComboBoxValue::CreateSP(this, &SPSIStringTableEntryHandleGraphPin::OnGetTableId),
                FOnPropertyComboBoxValueSelected::CreateSP(this, &SPSIStringTableEntryHandleGraphPin::OnSetTableId),
                /*bAllowClear*/ true, /*ShowSearchForItemCount*/ 20, /*bIncludeEmptyStringTables*/ true)
        ]
        + SWrapBox::Slot()
        [
            SNew(SBox)
            .Visibility(this, &SPSIStringTableEntryHandleGraphPin::GetKeySelectorVisibility)
            [
                FPSIStringTableEditorHelper::MakeStringTableKeySelector(
                    TAttribute<FName>::CreateSP(this, &SPSIStringTableEntryHandleGraphPin::GetStringTableIdName),
                    FOnGetPropertyComboBoxValue::CreateSP(this, &SPSIStringTableEntryHandleGraphPin::OnGetTableKey),
                    FOnPropertyComboBoxValueSelected::CreateSP(this, &SPSIStringTableEntryHandleGraphPin::OnSetTableKey))
            ]
        ];
}

FString SPSIStringTableEntryHandleGraphPin::OnGetTableId() const
{
    return FPSIStringTableHelper::GetStringTableDisplayString(StringTableEntryHandle.TableId);
}

void SPSIStringTableEntryHandleGraphPin::OnSetTableId(const FString& NewValue)
{
    FName TableId;
    const FStringTableConstPtr StringTable = FPSIStringTableHelper::FindStringTableByName(*NewValue, &TableId, /*bIncludeEmptyStringTables*/ true);
    if (StringTableEntryHandle.TableId == TableId)
    {
        return;
    }
    
    StringTableEntryHandle.TableId.Name = TableId;
    if (!StringTable.IsValid())
    {
        StringTableEntryHandle.Key.Reset();
    }
    
    else if (!StringTable->FindEntry(StringTableEntryHandle.Key))
    {
        StringTableEntryHandle.Key.Reset();
        StringTable->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
        {
            StringTableEntryHandle.Key = InKey;
            return false;
        });
    }

    UpdatePinDefaultValue();
}

FName SPSIStringTableEntryHandleGraphPin::GetStringTableIdName() const
{
    return StringTableEntryHandle.TableId;
}

FString SPSIStringTableEntryHandleGraphPin::OnGetTableKey() const
{
    return StringTableEntryHandle.Key;
}

void SPSIStringTableEntryHandleGraphPin::OnSetTableKey(const FString& String)
{
    if (StringTableEntryHandle.Key.Equals(String))
    {
        return;
    }
    
    const FStringTableConstPtr StringTablePtr = StringTableEntryHandle.GetStringTable();
    if (!StringTablePtr.IsValid() || !StringTablePtr->FindEntry(String).IsValid())
    {
        StringTableEntryHandle.Key.Reset();
    }

    else
    {
        StringTableEntryHandle.Key = String;
    }
    
    UpdatePinDefaultValue();
}

EVisibility SPSIStringTableEntryHandleGraphPin::GetKeySelectorVisibility() const
{
    return StringTableEntryHandle.TableId.IsNone() ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPSIStringTableEntryHandleGraphPin::UpdatePinDefaultValue()
{    
    const UScriptStruct* Struct = FPSIStringTableEntryHandle::StaticStruct();
    
    FString ValueAsString;
    Struct->ExportText(ValueAsString, &StringTableEntryHandle, nullptr, nullptr, PPF_SerializedAsImportText, nullptr);
    
    const FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
    if (!CurrentDefaultValue.Equals(ValueAsString))
    {
        const FScopedTransaction Transaction(LOCTEXT("ChangeDefaultValue", "Change Pin Default Value"));
        GraphPinObj->Modify();
        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, ValueAsString);
    }
}

#undef LOCTEXT_NAMESPACE