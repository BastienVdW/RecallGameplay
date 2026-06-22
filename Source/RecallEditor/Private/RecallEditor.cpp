// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallEditor.h"

#include "ISequencerModule.h"
#include "RecallLabelTrackEditor.h"

void FRecallEditor::StartupModule()
{
	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FRecallLabelTrackEditor::CreateTrackEditor));
}

IMPLEMENT_MODULE(FRecallEditor, RecallEditor);
