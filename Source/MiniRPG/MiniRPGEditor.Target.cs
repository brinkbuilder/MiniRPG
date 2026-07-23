using UnrealBuildTool;
using System.Collections.Generic;

public class MiniRPGEditorTarget : TargetRules
{
	public MiniRPGEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MiniRPG");
	}
}
