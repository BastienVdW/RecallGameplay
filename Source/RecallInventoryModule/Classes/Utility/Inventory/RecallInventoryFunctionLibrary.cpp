// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Inventory/RecallInventoryFunctionLibrary.h"

#include "Data/Inventory/RecallInventoryInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Interface/RecallPlayerControllerInterface.h"

void URecallInventoryFunctionLibrary::SetInventoryInputCommand(const UObject* WorldContextObject, FRecallInventoryInputCommand Command, int32 PlayerIndex)
{
	const TScriptInterface<IRecallPlayerControllerInterface> PlayerController(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
	if (PlayerController)
	{
		PlayerController->SetInputOptions(Command.ToOptions());
	}
}
