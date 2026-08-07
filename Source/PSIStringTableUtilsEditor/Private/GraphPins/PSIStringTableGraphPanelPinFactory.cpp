// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableGraphPanelPinFactory.h"
#include "EdGraphSchema_K2.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableId.h"
#include "SPSIStringTableEntryHandleGraphPin.h"
#include "SPSIStringTableIdGraphPin.h"

TSharedPtr<SGraphPin> FPSIStringTableGraphPanelPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if (const UScriptStruct* PinStructType = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject.Get()))
		{
			if (PinStructType->IsChildOf(FPSIStringTableId::StaticStruct()))
			{
				return SNew(SPSIStringTableIdGraphPin, InPin);
			}

			if (PinStructType->IsChildOf(FPSIStringTableEntryHandle::StaticStruct()))
			{
				return SNew(SPSIStringTableEntryHandleGraphPin, InPin);
			}
		}
	}
	
	return nullptr;
}
