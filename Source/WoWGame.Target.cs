using UnrealBuildTool;

public class WoWGameTarget : TargetRules
{
	public WoWGameTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new[]
		{
			"WoWGame",
			"WoWUI"
		});
	}
}