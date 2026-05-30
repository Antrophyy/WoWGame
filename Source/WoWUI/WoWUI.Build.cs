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
				"ModelViewViewModel",
				"Slate",
				"SlateCore",
				"UMG",

				// Game
				"GameplayMessageRuntime",
				"EazyUIInput",
				"EazyUI",
				"EazyUIMVVM",
				"GameIndicatorSystem",
				"GameLoadingScreen",
				"WoWGame",
				"WoWTags",
			}
		);
	}
}