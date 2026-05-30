using UnrealBuildTool;

public class EazyUI : ModuleRules
{
	public EazyUI(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
		[
			// Plugin
			"EazyUIInput",
			"EazyUIMVVM",

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
			"ModelViewViewModel",
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