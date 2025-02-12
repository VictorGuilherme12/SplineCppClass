using UnrealBuildTool;

public class HeliPhysicsPlugin : ModuleRules
{
	public HeliPhysicsPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore"
		});

		// Se precisar de outro módulo da Unreal, adicione aqui
	}
}