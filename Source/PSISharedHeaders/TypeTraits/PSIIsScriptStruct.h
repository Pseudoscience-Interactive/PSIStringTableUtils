// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include <type_traits>
#include <utility>

/** 
 * Template trait that checks if 'T' is a 'UScriptStruct', based on the existence of a 'StaticStruct()' function.
 */
template <typename T, typename = void>
struct TPSIIsScriptStruct
{
    static constexpr bool Value = false;
};

template <typename T>
struct TPSIIsScriptStruct<T, std::void_t<decltype(std::declval<T>().StaticStruct())>>
{
    static constexpr bool Value = true;
};
