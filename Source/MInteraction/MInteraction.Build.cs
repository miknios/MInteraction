// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MInteraction : ModuleRules
{
	public MInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"MUtility",
				"MPointSelectorQuery",
				"SDFutureExtensions",

				"Core",
				"DeveloperSettings",
				"UMG",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}