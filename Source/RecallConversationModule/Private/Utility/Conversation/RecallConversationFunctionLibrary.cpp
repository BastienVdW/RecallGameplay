// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Conversation/RecallConversationFunctionLibrary.h"

#include "ConversationTypes.h"
#include "Conversation/RecallConversationInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Interface/RecallPlayerControllerInterface.h"

void URecallConversationFunctionLibrary::SetInputConversationCommand(const UObject* WorldContextObject,
	FConversationChoiceReference ChoiceReference, int32 PlayerIndex /*= 0*/)
{
	FRecallConversationInputCommand InputCommand;
	InputCommand.Type = ERecallConversationInputType::Choice;
	InputCommand.NodeGUID = ChoiceReference.NodeReference.NodeGUID;
	
	const TScriptInterface<IRecallPlayerControllerInterface> PlayerController(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
	if (PlayerController)
	{
		PlayerController->SetInputOptions(InputCommand.ToOptions());
	}
}
