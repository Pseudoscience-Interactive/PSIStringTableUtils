// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

/** 
 * Can be added to the private section of a class to allow 'MakeShared<T>' to work despite a private or protected
 * constructor 
 */
#define PSI_ALLOW_MAKE_SHARED \
template <typename ObjectType, ESPMode Mode> friend class SharedPointerInternals::TIntrusiveReferenceController;