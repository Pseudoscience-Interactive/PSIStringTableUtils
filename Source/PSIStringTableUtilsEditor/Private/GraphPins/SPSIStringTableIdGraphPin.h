// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableId.h"
#include "KismetPins/SGraphPinStructInstance.h"

/**
 * Widget class for pins of the 'FPSIStringTableId'.
 */
class SPSIStringTableIdGraphPin final : public SGraphPinStructInstance
{
public:
    SLATE_BEGIN_ARGS(SPSIStringTableIdGraphPin) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin);
    
protected:
    //~ Begin SGraphPinStructInstance Interface
    virtual void ParseDefaultValueData() override;
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
    //~ End SGraphPinStructInstance Interface
    
private:
    FString OnGetTableIdValue() const;
    void OnSetTableIdValue(const FString& NewValue);
    
    FPSIStringTableId StringTableId;
};
