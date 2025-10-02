// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LegoConnectionToolEditor : ModuleRules
{
	public LegoConnectionToolEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"InputCore",
                "Slate",    		
                "SlateCore",		
				"PropertyEditor", 	// For Customizing the Details Panel of an Actor
				"LegoConnectionTool", 		
                "Json", 			// For Serialization
                "JsonUtilities"
			}
		);
	}
}
