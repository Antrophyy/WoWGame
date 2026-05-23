using UnrealBuildTool;

public class WoWTags : ModuleRules
{
    public WoWTags(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayTags"
            }
        );
    }
}