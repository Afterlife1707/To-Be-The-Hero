// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ThirdPersonClientTarget : TargetRules
{
	public ThirdPersonClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("ThirdPerson");
	}
}
