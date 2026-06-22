// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationGenericSideEffects.h"

#include "ConversationContext.h"
#include "System/Conversation/RecallConversationSubsystem.h"

//----------------------------------------------------------------------//
// URecallConversationTriggerSideEffect
//----------------------------------------------------------------------//
void URecallConversationTriggerSideEffect::CauseSideEffect_Implementation(
	const FConversationContext& Context) const
{
	URecallConversationSubsystem* ConversationSystem = UWorld::GetSubsystem<URecallConversationSubsystem>(Context.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(ConversationSystem),
		TEXT("%hs Invalid conversation system"), __FUNCTION__))
	{
		return;
	}

	ConversationSystem->PushConversationTriggers(ConversationTriggers);
}
