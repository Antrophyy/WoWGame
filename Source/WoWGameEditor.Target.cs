using UnrealBuildTool;

public class WoWGameEditorTarget : TargetRules
{
	public WoWGameEditorTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new[]
		{
			"WoWGame",
			"WoWUI"
		});
	}
}