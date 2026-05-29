using UnrealBuildTool;

public class WoWUI : ModuleRules
{
	public WoWUI(ReadOnlyTargetRules target) : base(target)
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
				"Engine",
				"EnhancedInput",
				"GameplayTags",
				"InputCore",
				"Slate",
				"SlateCore",
				"UMG",

				// Game
				"GameplayMessageRuntime",
				"EazyUIInput",
				"EazyUI",
				"GameIndicatorSystem",
				"GameLoadingScreen",
				"WoWGame",
				"WoWTags",
			}
		);
	}
}