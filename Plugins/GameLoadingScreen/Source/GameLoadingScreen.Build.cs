using UnrealBuildTool;

public class GameLoadingScreen : ModuleRules
{
	public GameLoadingScreen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new[]
		{
			// Engine
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"GameplayTags",
			"InputCore",
			"MoviePlayer",
			"PreLoadScreen",
			"RenderCore",
			"Slate",
			"SlateCore",
			"UMG", 
		});
	}
}
