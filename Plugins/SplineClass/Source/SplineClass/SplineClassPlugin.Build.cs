// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SplineClassPlugin : ModuleRules
{
	public SplineClassPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore", // Adicionado caso precise de input
				"GameplayAbilities" // Alternativa para lidar com splines se necessário
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UMG", // Caso use interface gráfica
				"PhysicsCore", // Para física avançada, se necessário
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}