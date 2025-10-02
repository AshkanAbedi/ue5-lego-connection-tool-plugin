// Copyright Epic Games, Inc. All Rights Reserved.
#include "LegoConnectionToolEditor.h"
#include "LegoActor.h"
#include "LegoActorDetailsCustomization.h"
#include "PropertyEditorModule.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FLegoConnectionToolEditorModule"

TSharedPtr<FSlateStyleSet> LegoStyleSet;

void FLegoConnectionToolEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		ALegoActor::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FLegoActorDetailsCustomization::MakeInstance)
	);

	LegoStyleSet = MakeShared<FSlateStyleSet>("LegoConnectionToolStyle");

	const FString ContentDir = FPaths::ProjectPluginsDir() / TEXT("LegoConnectionTool/Resources");
	
	LegoStyleSet->Set("LegoConnectionTool.Shape.Cube",
		new FSlateVectorImageBrush(ContentDir / TEXT("Cube.svg"), FVector2D(24.f, 24.f)));
	LegoStyleSet->Set("LegoConnectionTool.Shape.Sphere",
		new FSlateVectorImageBrush(ContentDir / TEXT("Sphere.svg"), FVector2D(24.f, 24.f)));
	LegoStyleSet->Set("LegoConnectionTool.Shape.Capsule",
		new FSlateVectorImageBrush(ContentDir / TEXT("Capsule.svg"), FVector2D(24.f, 24.f)));
	LegoStyleSet->Set("LegoConnectionTool.Shape.Cone",
		new FSlateVectorImageBrush(ContentDir / TEXT("Cone.svg"), FVector2D(24.f, 24.f)));

	FSlateStyleRegistry::RegisterSlateStyle(*LegoStyleSet);
	
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