// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ToBeTheHeroServerTarget : TargetRules
{
	public ToBeTheHeroServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        bUsesSteam = true;
        ExtraModuleNames.Add("ToBeTheHero");
	}
}
