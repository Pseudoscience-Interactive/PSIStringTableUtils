// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "Misc/TVariantMeta.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// IsScriptStruct

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

///////////////////////////////////////////////////////////////////////////////////////////////
// IsSame

/**
 * Implementation of 'std::is_same' trait. This is needed because 'TIsSame' in 'UnrealTypeTraits.h' has been
 * deprecated in 5.2 (apparently for no reason). We need this trait, though, because 'std::is_same' does not work with
 * 'TAnd','TNot', etc. 
 */
template<typename A, typename B>
struct TPSISpreadsheetIsSame
{
    enum { Value = std::is_same_v<A, B>    };
};


///////////////////////////////////////////////////////////////////////////////////////////////
// IsUEnumClass

enum EHorizontalAlignment : int;
enum EVerticalAlignment : int;

/**
 * Note that there are certain 'UENUM' types where 'TIsUEnumClass' returns false. I suspect that is true for all
 * these enum types whose underlying integer type is not 'uint8'. We must mention all these types explicitly in
 * order to use them.
 */
template <typename T>
struct TPSIIsUEnumClass
{
    enum
    {
        Value = TOr<TIsUEnumClass<T>,
                    TPSISpreadsheetIsSame<T, EHorizontalAlignment>,
                    TPSISpreadsheetIsSame<T, EVerticalAlignment>>::Value
    };
};

///////////////////////////////////////////////////////////////////////////////////////////////
// VariantHasType

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