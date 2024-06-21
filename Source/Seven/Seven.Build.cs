// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Seven : ModuleRules
{
	public Seven(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        OptimizeCode = CodeOptimization.Never;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara", "MotionWarping", "Paper2D" });
	}
}
