using UnrealBuildTool;

public class EazyUIInput : ModuleRules
{
	public EazyUIInput(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
		[
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
		]);
	}
}