using UnrealBuildTool;
using System.Collections.Generic;

public class MiniRPGTarget : TargetRules
{
	public MiniRPGTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MiniRPG");
	}
}
