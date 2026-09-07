// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Internationalization/StringTableCoreFwd.h"

/**
 * Test-only RAII fixture that registers a transient 'FStringTable' under a unique table ID for the
 * lifetime of the scope, and unregisters it again on destruction.
 */
struct FPSIStringTableUtilsTestTableScope
{
    explicit FPSIStringTableUtilsTestTableScope(const FString& DebugName = TEXT("PSIStringTableUtilsTest"));
    ~FPSIStringTableUtilsTestTableScope();

    FPSIStringTableUtilsTestTableScope(const FPSIStringTableUtilsTestTableScope&) = delete;
    FPSIStringTableUtilsTestTableScope& operator=(const FPSIStringTableUtilsTestTableScope&) = delete;

    /** Sets the source string of the given entry, hiding the 'WITH_EDITORONLY_DATA'-dependent dev-notes argument. */
    void SetSourceString(const FString& Key, const FString& SourceString) const;

    FName TableId;
    FStringTableRef StringTable;
};
