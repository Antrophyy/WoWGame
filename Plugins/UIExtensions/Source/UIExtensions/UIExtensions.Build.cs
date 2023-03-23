using UnrealBuildTool;

public class UIExtensions : ModuleRules
{
	public UIExtensions(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				// Engine
				"ModularGameplay",

				// Game
				"AsyncMixin",
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				// Engine
				"CommonInput",
				"CommonUI",
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"Slate",
				"SlateCore",
				"UMG",
				
				// Game
			});
	}
}