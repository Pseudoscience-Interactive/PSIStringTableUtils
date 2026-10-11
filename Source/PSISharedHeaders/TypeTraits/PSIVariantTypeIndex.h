// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/TVariantMeta.h"

template <typename T, typename... Ts>
class TVariant;

/** Determines the index of a specific type inside a 'TVariant'.  */
template <typename TypeToCheck, typename T>
struct TPSIVariantTypeIndex
{
    static constexpr SIZE_T Value = INDEX_NONE;
};

template <typename TypeToCheck, typename T, typename... Ts>
struct TPSIVariantTypeIndex<TypeToCheck, TVariant<T, Ts...>>
{
    static constexpr SIZE_T Value = UE::Core::Private::TParameterPackTypeIndex<TypeToCheck, T, Ts...>::Value;
};
