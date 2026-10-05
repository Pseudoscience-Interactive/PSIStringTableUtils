// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableUtilsTestFixture.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "Misc/Guid.h"
#include "PSIEngineVersionComparison.h"

FPSIStringTableUtilsTestTableScope::FPSIStringTableUtilsTestTableScope(const FString& DebugName)
    : TableId(*FString::Printf(TEXT("%s_%s"), *DebugName, *FGuid::NewGuid().ToString(EGuidFormats::Digits)))
    , StringTable(FStringTable::NewStringTable())
{
    FStringTableRegistry::Get().RegisterStringTable(TableId, StringTable);
}

FPSIStringTableUtilsTestTableScope::~FPSIStringTableUtilsTestTableScope()
{
    FStringTableRegistry::Get().UnregisterStringTable(TableId);
}

void FPSIStringTableUtilsTestTableScope::SetSourceString(const FString& Key, const FString& SourceString) const
{
#if PSI_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0) && WITH_EDITORONLY_DATA
    StringTable->SetSourceString(Key, SourceString, FString());
#else
    StringTable->SetSourceString(Key, SourceString);
#endif
}
