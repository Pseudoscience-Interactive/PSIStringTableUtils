// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

#include "PSIStringTableUtilsEditorModule.h"
#include "EdGraphUtilities.h"
#include "PropertyEditorModule.h"
#include "PSIStringTableEntryHandle.h"
#include "PSIStringTableId.h"
#include "GraphPins/PSIStringTableGraphPanelPinFactory.h"
#include "Modules/ModuleManager.h"
#include "PropertyTypeCustomizations/PSIStringTableEntryHandleTypeCustomization.h"
#include "PropertyTypeCustomizations/PSIStringTableIdTypeCustomization.h"

class FPSIStringTableUtilsEditorModule : public IPSIStringTableUtilsEditorModule
{
public:
    //~ Begin IModuleInterface Interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    //~ End IModuleInterface Interface
    
private:
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
};

const FName FPSIStringTableUtilsEditorModule::PropertyEditorName(TEXT("PropertyEditor"));

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
}

void FPSIStringTableUtilsEditorModule::ShutdownModule()
{
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

IMPLEMENT_MODULE(FPSIStringTableUtilsEditorModule, PSIStringTableUtilsEditor)