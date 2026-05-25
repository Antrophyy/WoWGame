using UnrealBuildTool;

public class RareInput : ModuleRules
{
	public RareInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CommonInput",
				"CommonUI",
				"Core",
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"InputCore",
				"Slate",
				"SlateCore",
				"UMG"
			});
	}
}
