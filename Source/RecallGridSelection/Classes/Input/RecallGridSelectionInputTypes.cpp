// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGridSelectionInputTypes.h"

#include "Utility/Input/RecallInputUtils.h"

#define RECALL_GRID_SELECTION_INPUT_TYPE_OPTION				FString("GridSelectionInputType")
#define RECALL_GRID_SELECTION_INPUT_COORDINATES_OPTION		FString("GridSelectionInputCoordinates")

FRecallGridSelectionInputCommand::FRecallGridSelectionInputCommand(const FString& Options)
{
	Type = Recall::Input::Utils::GetEnumOption<ERecallGridSelectionInputType>(
		Options, RECALL_GRID_SELECTION_INPUT_TYPE_OPTION, ERecallGridSelectionInputType::None);

	if (Type != ERecallGridSelectionInputType::None)
	{
		const FString CoordinatesStr = Recall::Input::Utils::GetOption(
			Options, RECALL_GRID_SELECTION_INPUT_COORDINATES_OPTION);
		if (!CoordinatesStr.IsEmpty())
		{
			GridPosition.InitFromString(CoordinatesStr);
		}
	}
}

FString FRecallGridSelectionInputCommand::ToOptions() const
{
	FString Options;

	Recall::Input::Utils::AddEnumOption<ERecallGridSelectionInputType>(RECALL_GRID_SELECTION_INPUT_TYPE_OPTION, Options, Type);
	Recall::Input::Utils::AddOption(RECALL_GRID_SELECTION_INPUT_COORDINATES_OPTION, Options, GridPosition.ToString());

	return Options;
}
