// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableUtilsModule.h"
#include "Modules/ModuleManager.h"
#include "PSIStringTableUtilsLog.h"

class FPSIStringTableUtilsModule : public IPSIStringTableUtilsModule
{
public:
    //~ Begin IModuleInterface Interface
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
    //~ End IModuleInterface Interface
};


IMPLEMENT_MODULE(FPSIStringTableUtilsModule, PSIStringTableUtils)

DEFINE_LOG_CATEGORY(LogPSIStringTableUtils);
