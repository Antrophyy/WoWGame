using UnrealBuildTool;

public class EazyUI : ModuleRules
{
	public EazyUI(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
		[
			// Game
			"EazyUIInput",

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
		]);
		
		if (target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
			[
				// Engine
				"UnrealEd"
			]);
		}
	}
}