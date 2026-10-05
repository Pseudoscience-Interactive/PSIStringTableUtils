// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "PSIVariantTypeIndex.h"

template <typename T, typename... Ts>
class TVariant;

/** Determines whether a 'TVariant' contains a specific type. */
template <typename TypeToCheck, typename T>
struct TPSIVariantHasType
{
    static constexpr bool Value = false;
};

template <typename TypeToCheck, typename T, typename... Ts>
struct TPSIVariantHasType<TypeToCheck, TVariant<T, Ts...>>
{
    static constexpr bool Value = TPSIVariantTypeIndex<TypeToCheck, TVariant<T, Ts...>>::Value != INDEX_NONE;
};
