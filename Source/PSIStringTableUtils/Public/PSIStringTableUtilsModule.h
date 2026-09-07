 // Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class UStringTable;

/** Broadcast whenever a String Table asset is changed through this plugin's Blueprint API, outside of its own native editor. */
DECLARE_DELEGATE_OneParam(FPSIOnStringTableChangedExternally, UStringTable* /*OwnerAsset*/);

/**
 * Interface for the PSI String Table Utilities module.
 */
class IPSIStringTableUtilsModule : public IModuleInterface
{
public:
    static IPSIStringTableUtilsModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IPSIStringTableUtilsModule>(TEXT("PSIStringTableUtils"));
    }

    /** Bound by 'PSIStringTableUtilsEditor' to refresh an already-open native String Table editor. */
    FPSIOnStringTableChangedExternally OnStringTableChangedExternally;
};
