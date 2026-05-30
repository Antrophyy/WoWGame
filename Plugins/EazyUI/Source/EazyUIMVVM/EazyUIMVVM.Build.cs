using UnrealBuildTool;

public class EazyUIMVVM : ModuleRules
{
    public EazyUIMVVM(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            [
                // Engine
                "Core",
                "CoreUObject",
                "Engine",
                "ModelViewViewModel",
                "Slate",
                "SlateCore",
                "UMG",
            ]
        );
    }
}