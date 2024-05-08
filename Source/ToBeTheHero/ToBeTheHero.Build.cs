// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ToBeTheHero : ModuleRules
{
	public ToBeTheHero(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemSteam", "PlayFabGSDK", "PlayFab", "PlayFabCpp", "PlayFabCommon", "UMG" });
    }
}
