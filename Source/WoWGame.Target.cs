using UnrealBuildTool;

public class WoWGameTarget : TargetRules
{
	public WoWGameTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new[]
		{
			"WoWGame",
			"WoWUI"
		});
	}
}