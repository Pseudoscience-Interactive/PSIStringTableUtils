// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "Misc/EngineVersionComparison.h"

// 'TEMPLATE_REQUIRES' has been deprecated in UE_5.5 and has been replaced with 'UE_REQUIRES'
// We define the 'PSI_REQUIRES' macro to account for that.
#if !UE_VERSION_OLDER_THAN(5, 5, 0)
    #define PSI_REQUIRES(...) UE_REQUIRES(__VA_ARGS__)
#else
    // Note that we add a comma in front of 'TEMPLATE_REQUIRES' to match the syntax of 'UE_REQUIRES'
    #define PSI_REQUIRES(...) , TEMPLATE_REQUIRES(__VA_ARGS__)
#endif