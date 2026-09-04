// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PSIStringTableUtilsTestObjects.generated.h"

/** Test-only struct whose property names are used as metadata IDs by the meta-data-as-struct tests. */
USTRUCT()
struct FPSIStringTableUtilsTestMetaDataStruct
{
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    int32 Count = 0;
};
