// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ThirdPersonServerTarget : TargetRules
{
	public ThirdPersonServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("ThirdPerson");

        DisablePlugins.Add("WMFMediaPlayer");
        DisablePlugins.Add("AsyncLoadingScreen");
        DisablePlugins.Add("WindowsMoviePlayer");
        DisablePlugins.Add("MediaFoundationMediaPlayer");
    }
}
