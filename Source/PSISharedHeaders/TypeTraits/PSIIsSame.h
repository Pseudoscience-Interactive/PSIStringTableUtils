    // Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include <type_traits>

/**
 * Implementation of 'std::is_same' trait. This is needed because 'TIsSame' in 'UnrealTypeTraits.h' has been
 * deprecated in 5.2 (apparently for no reason). We need this trait, though, because 'std::is_same' does not work with
 * 'TAnd','TNot', etc. 
 */
template<typename A, typename B>
struct TPSIIsSame
{
    enum { Value = std::is_same_v<A, B>    };
};
