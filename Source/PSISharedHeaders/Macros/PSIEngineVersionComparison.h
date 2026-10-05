// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "Misc/EngineVersionComparison.h"

// Version comparison macro that is defined to true if the UE version is newer or equal than MajorVer.MinorVer.PatchVer 
// and false otherwise (a typical use is for backward compatible code).
//
// Same as 'UE_VERSION_NEWER_THAN_OR_EQUAL' in 'EngineVersionComparison.h' because the original version does not exist 
// before UE 5.6.
#define PSI_VERSION_NEWER_THAN_OR_EQUAL(MajorVersion, MinorVersion, PatchVersion)\
UE_GREATER_SORT(ENGINE_MAJOR_VERSION, MajorVersion, UE_GREATER_SORT(ENGINE_MINOR_VERSION, MinorVersion, UE_GREATER_SORT(ENGINE_PATCH_VERSION, PatchVersion, true)))