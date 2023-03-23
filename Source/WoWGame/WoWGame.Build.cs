using UnrealBuildTool;

public class WoWGame : ModuleRules
{
	public WoWGame(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new[]
		{
			// Engine
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",

			// Game
			"GameplayMessageRuntime",
		});
	}
}