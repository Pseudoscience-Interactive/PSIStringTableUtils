// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableEntryHandle.h"
#include "KismetPins/SGraphPinStructInstance.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Widget class for pins of the 'FPSIStringTableEntryHandle' struct.
 */
class SPSIStringTableEntryHandleGraphPin final : public SGraphPinStructInstance
{
public:
    SLATE_BEGIN_ARGS(SPSIStringTableEntryHandleGraphPin) {}

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin);
    
protected:
    //~ Begin SGraphPinStructInstance Interface
    virtual void ParseDefaultValueData() override;
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
    //~ End SGraphPinStructInstance Interface
    
private:
    FString OnGetTableId() const;
    void OnSetTableId(const FString& NewValue);
    FName GetStringTableIdName() const;
    FString OnGetTableKey() const;
    void OnSetTableKey(const FString& String);
    
    void UpdatePinDefaultValue();
    
    FPSIStringTableEntryHandle StringTableEntryHandle;
};
