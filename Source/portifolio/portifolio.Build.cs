// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class portifolio : ModuleRules
{
	public portifolio(ReadOnlyTargetRules Target) : base(Target)
	{
        //MinFilesUsingPrecompiledHeaderOverride = 1;
        //bFasterWithoutUnity = true;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
