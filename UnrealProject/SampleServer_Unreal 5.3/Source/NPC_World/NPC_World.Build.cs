// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NPC_World : ModuleRules
{
	public NPC_World(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","Networking","Sockets", "ProceduralMeshComponent", "Json", "JsonUtilities" });
	}
}
