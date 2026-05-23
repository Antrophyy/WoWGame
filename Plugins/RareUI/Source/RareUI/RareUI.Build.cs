// Copyright (C) Grip Studios. All Rights Reserved

using UnrealBuildTool;

public class RareUI : ModuleRules
{
	public RareUI(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				// Engine
				"CommonInput",
				"CommonUI",
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"EnhancedInput",
				"GameplayTags",
				"InputCore",
				"Slate",
				"SlateCore",
				"UMG"
			});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					// Engine
					"UnrealEd"
				});
		}
	}
}