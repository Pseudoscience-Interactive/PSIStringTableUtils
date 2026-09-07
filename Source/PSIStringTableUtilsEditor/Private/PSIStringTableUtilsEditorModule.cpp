// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableUtilsEditorModule.h"
#include "Editor.h"
#include "EdGraphUtilities.h"
#include "IStringTableEditor.h"
#include "Internationalization/StringTable.h"
#include "PropertyEditorModule.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableId.h"
#include "PSIStringTableUtilsModule.h"
#include "GraphPins/PSIStringTableGraphPanelPinFactory.h"
#include "Modules/ModuleManager.h"
#include "PropertyTypeCustomizations/PSIStringTableEntryHandleTypeCustomization.h"
#include "PropertyTypeCustomizations/PSIStringTableIdTypeCustomization.h"
#include "Subsystems/AssetEditorSubsystem.h"

class FPSIStringTableUtilsEditorModule : public IPSIStringTableUtilsEditorModule
{
public:
    //~ Begin IModuleInterface Interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    //~ End IModuleInterface Interface
    
private:
    /** Refreshes an already-open native String Table editor after 'OwnerAsset' was changed through our Blueprint API. */
    void OnStringTableChangedExternally(UStringTable* OwnerAsset);

    void RegisterCustomPropertyTypeLayout(FName ClassName,
                                          const FOnGetPropertyTypeCustomizationInstance& PropertyTypeLayoutDelegate,
                                          const TSharedPtr<IPropertyTypeIdentifier>& PropertyTypeIdentifier =
                                              nullptr)
    {
        check(ClassName != NAME_None)
        check(PropertyTypeLayoutDelegate.IsBound())

        FPropertyTypeCustomizationData& Data = RegisteredPropertyTypeCustomizations.AddDefaulted_GetRef();
        Data.ClassName = ClassName;
        Data.PropertyTypeIdentifier = PropertyTypeIdentifier;

        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
            PropertyEditorName);
        PropertyModule.RegisterCustomPropertyTypeLayout(ClassName, PropertyTypeLayoutDelegate, PropertyTypeIdentifier);
    }
    
    /** List of registered class names for 'IDetailCustomizations' that we must unregister when the module shuts down */
    TSet<FName> RegisteredDetailCustomizations;
    
    struct FPropertyTypeCustomizationData
    {
        FName ClassName;
        TSharedPtr<IPropertyTypeIdentifier> PropertyTypeIdentifier;
    };
    
    /** List of registered type names for 'IPropertyTypeCustomizations' that we must unregister when the module shuts down */
    TArray<FPropertyTypeCustomizationData> RegisteredPropertyTypeCustomizations;
    
    TSharedPtr<FPSIStringTableGraphPanelPinFactory> GraphPanelPinFactory;

    static const FName PropertyEditorName;
    static const FName StringTableUtilsModuleName;
};

const FName FPSIStringTableUtilsEditorModule::PropertyEditorName(TEXT("PropertyEditor"));
const FName FPSIStringTableUtilsEditorModule::StringTableUtilsModuleName(TEXT("PSIStringTableUtils"));

void FPSIStringTableUtilsEditorModule::StartupModule()
{
    // Register property type customizations
    //
    RegisterCustomPropertyTypeLayout(
        FPSIStringTableId::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(FPSIStringTableIdTypeCustomization::MakeInstance));

    RegisterCustomPropertyTypeLayout(
        FPSIStringTableEntryHandle::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(FPSIStringTableEntryHandleTypeCustomization::MakeInstance));
    
    // Register the graph pin factory
    //
    GraphPanelPinFactory = MakeShared<FPSIStringTableGraphPanelPinFactory>();
    FEdGraphUtilities::RegisterVisualPinFactory(GraphPanelPinFactory);

    // Listen for String Table changes made through 'PSIStringTableUtils's Blueprint API, so an already-open
    // native String Table editor can be refreshed. That refresh logic needs editor-only types this module's
    // base module may not depend on, so it is reached through a delegate instead.
    //
    IPSIStringTableUtilsModule::Get().OnStringTableChangedExternally.BindRaw(
        this, &FPSIStringTableUtilsEditorModule::OnStringTableChangedExternally);
}

void FPSIStringTableUtilsEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded(StringTableUtilsModuleName))
    {
        IPSIStringTableUtilsModule::Get().OnStringTableChangedExternally.Unbind();
    }

    // Unregister the detail customizations and the property type customizations
    //
    if (FModuleManager::Get().IsModuleLoaded(PropertyEditorName))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
            PropertyEditorName);

        for (const FPropertyTypeCustomizationData& CustomizationData : RegisteredPropertyTypeCustomizations)
        {
            PropertyModule.UnregisterCustomPropertyTypeLayout(CustomizationData.ClassName, CustomizationData.PropertyTypeIdentifier);
        }
    }

    // Unregister our pin factory again.
    FEdGraphUtilities::UnregisterVisualPinFactory(GraphPanelPinFactory);
}

void FPSIStringTableUtilsEditorModule::OnStringTableChangedExternally(UStringTable* OwnerAsset)
{
    if (OwnerAsset == nullptr || GEditor == nullptr)
    {
        return;
    }

    const TArray<IAssetEditorInstance*> Editors =
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorsForAsset(OwnerAsset);
    IAssetEditorInstance* const* ExistingInstance = Editors.FindByPredicate([](const IAssetEditorInstance* Editor)
    {
        return Editor->GetEditorName() == FName(TEXT("StringTableEditor"));
    });

    if (ExistingInstance != nullptr)
    {
        static_cast<IStringTableEditor*>(*ExistingInstance)->RefreshStringTableEditor();
    }
}

IMPLEMENT_MODULE(FPSIStringTableUtilsEditorModule, PSIStringTableUtilsEditor)