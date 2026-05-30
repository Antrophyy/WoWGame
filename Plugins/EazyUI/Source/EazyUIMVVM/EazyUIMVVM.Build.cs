using UnrealBuildTool;

public class EazyUIMVVM : ModuleRules
{
    public EazyUIMVVM(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            [
                // Engine
                "Core",
                "CoreUObject",
                "Engine",
                "ModelViewViewModel",
                "UMG"
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                // Engine
                "Slate",
                "SlateCore"
            ]
        );
    }
}