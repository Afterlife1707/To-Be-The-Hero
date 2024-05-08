// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ToBeTheHeroTarget : TargetRules
{
	public ToBeTheHeroTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		bUsesSteam = true;
		ExtraModuleNames.Add("ToBeTheHero");
	}
}
