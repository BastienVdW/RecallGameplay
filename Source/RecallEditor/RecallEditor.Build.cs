// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

using UnrealBuildTool;

public class RecallEditor : ModuleRules
{
	public RecallEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Default;

		PublicDependencyModuleNames.AddRange(new string[] {
			"RHI",
			"Core",
			"CoreUObject",
			"Engine",
			"Sequencer",
			"EditorStyle",
			"SequencerCore",
			"MovieScene",
			"MovieSceneTracks",
			"RecallCore",
			"RecallGameplay",
			"UnrealEd",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "JsonUtilities", "PythonScriptPlugin" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "ToolWidgets", "EditorWidgets", "UMG" });
		
	}
}
