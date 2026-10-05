// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "PSIIsSame.h"
#include "Templates/AndOrNot.h"
#include "Templates/IsUEnumClass.h"

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
                    TPSIIsSame<T, EHorizontalAlignment>,
                    TPSIIsSame<T, EVerticalAlignment>>::Value
    };
};
