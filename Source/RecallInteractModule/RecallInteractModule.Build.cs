// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

using System;
using System.IO;
using UnrealBuildTool;

public class RecallInteractModule : ModuleRules
{
	public RecallInteractModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Engine", 
			"VariableCollectionModule", 
			"RecallCore", 
			"RecallGameplayCore",
		});
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"MassExtendedEntity",
			"MassExtendedSpawner",
			"RecallSimulation",
			"RecallPhysicsModule",
			"RecallSignals",
			"RecallGameplay",
			"RecallAttributeModule",
			"RecallGameplayEffectModule",
			"GameplayTags",
			"StateTreeModule",
			"PropertyBindingUtils",
		});
	}
}