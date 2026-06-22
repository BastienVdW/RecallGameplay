// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractConversationCommandTypes.h"

#include "Utility/Conversation/RecallConversationUtils.h"

//----------------------------------------------------------------------//
// FRecallConversationCommand:
//----------------------------------------------------------------------//
bool FRecallConversationCommand::Validate(const FRecallInteractContext& Context) const
{
	return Recall::Conversation::Utils::CanStartConversation(Context.ExecutionContext,
		Context.InstigatorEntity, Context.InteractableEntity,
		ConversationEntryPoint, ConversationEntryPointIdentifier);
}

void FRecallConversationCommand::OnExecute(const FRecallInteractContext& Context) const
{
	Recall::Conversation::Utils::StartConversation(Context.ExecutionContext,
		Context.InstigatorEntity, Context.InteractableEntity,
		ConversationEntryPoint, ConversationEntryPointIdentifier);
}
