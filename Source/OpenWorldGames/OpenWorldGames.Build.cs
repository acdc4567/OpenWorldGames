// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenWorldGames : ModuleRules
{
	public OpenWorldGames(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","UMG" ,"NavigationSystem","AIModule"});

		PublicIncludePaths.AddRange(new string[] {
		"OpenWorldGames/Public/Character"
		,"OpenWorldGames/Public/Animation"
		,"OpenWorldGames/Public/Weapons"
		,"OpenWorldGames/Public/Interfaces"
		,"OpenWorldGames/Public/PlayerControllers"
		,"OpenWorldGames/Public/Enemies"
		});



	}
}
