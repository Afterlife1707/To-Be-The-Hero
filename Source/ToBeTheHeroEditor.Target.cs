// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ToBeTheHeroEditorTarget : TargetRules
{
	public ToBeTheHeroEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        bUsesSteam = true;
        ExtraModuleNames.Add("ToBeTheHero");
	}
}
