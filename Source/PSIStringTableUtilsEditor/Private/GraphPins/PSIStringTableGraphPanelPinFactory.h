// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinStructInstance.h"

struct FPSIStringTableGraphPanelPinFactory : FGraphPanelPinFactory
{
    //~ Begin FGraphPanelPinFactory Implementation
    virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* InPin) const override;
    //~ End FGraphPanelPinFactory Implementation
};