// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ToBeTheHeroClientTarget : TargetRules
{
	public ToBeTheHeroClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        bUsesSteam = true;
        ExtraModuleNames.Add("ToBeTheHero");
	}
}
