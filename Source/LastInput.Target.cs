using UnrealBuildTool;
using System.Collections.Generic;

public class LastInputTarget : TargetRules
{
	public LastInputTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("LastInput");
	}
}
