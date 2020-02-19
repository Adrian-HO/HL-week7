// Copyright (c) 2017 GAM1528. All Rights Reserved.

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem", "UMG" });
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
