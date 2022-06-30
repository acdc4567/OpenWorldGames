// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenWorldGames : ModuleRules
{
	public OpenWorldGames(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
