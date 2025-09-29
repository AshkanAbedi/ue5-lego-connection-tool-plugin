// Copyright Epic Games, Inc. All Rights Reserved.

#include "LegoConnectionToolEditor.h"
#include "LegoActor.h"
#include "LegoActorDetailsCustomization.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FLegoConnectionToolEditorModule"

void FLegoConnectionToolEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		ALegoActor::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FLegoActorDetailsCustomization::MakeInstance)
	);
}

void FLegoConnectionToolEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(ALegoActor::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLegoConnectionToolEditorModule, LegoConnectionToolEditor)