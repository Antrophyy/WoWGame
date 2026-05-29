using UnrealBuildTool;

public class GameIndicatorSystem : ModuleRules
{
	public GameIndicatorSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Engine
				"Core", 
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
			}
		);
	}
}
