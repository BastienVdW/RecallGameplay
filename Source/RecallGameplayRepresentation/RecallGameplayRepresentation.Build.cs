// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

namespace UnrealBuildTool.Rules
{
	public class RecallGameplayRepresentation : ModuleRules
	{
		public RecallGameplayRepresentation(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new string[] {
				"Engine", 
				"MassEntity", 
				"RecallCore", 
				"RecallGameplayCore"
			});
			
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Core",
					"CoreUObject",
					"RecallFrontend",
					"RecallSimulation",
					"RecallGameplay",
					"RecallGameRuleModule",
					"RecallConversationModule",
					"RecallAttributeModule",
					"RecallInteractModule",
					"RecallInventoryModule",
					"CommonUI",
					"EnhancedInput",
					"ExtendedCommonUI",
					"CommonGame",
					"GameplayTags",
					"CommonConversationRuntime",
				}
			);

			if (Target.bBuildEditor == true)
			{
				PrivateDependencyModuleNames.Add("EditorFramework");
				PrivateDependencyModuleNames.Add("UnrealEd");
			}

			// Uncomment if you are using Slate UI
			PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });
		}
	}
}
