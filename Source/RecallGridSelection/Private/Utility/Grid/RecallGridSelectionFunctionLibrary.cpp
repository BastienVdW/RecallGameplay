// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Grid/RecallGridSelectionFunctionLibrary.h"

#include "Input/RecallGridSelectionInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Interface/RecallPlayerControllerInterface.h"

void URecallGridSelectionFunctionLibrary::SetGridSelectionInputCommand(const UObject* WorldContextObject,
	FRecallGridSelectionInputCommand Command, int32 PlayerIndex)
{
	const TScriptInterface<IRecallPlayerControllerInterface> PlayerController(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
	if (PlayerController)
	{
		// Do not override a select command with a deselect command
		const FRecallGridSelectionInputCommand PreviousInputCommand(PlayerController->GetInputOptions());
		if (PreviousInputCommand.IsSelect() && Command.IsDeselect())
		{
			return;
		}
		
		PlayerController->SetInputOptions(Command.ToOptions(), true);
	}
}
