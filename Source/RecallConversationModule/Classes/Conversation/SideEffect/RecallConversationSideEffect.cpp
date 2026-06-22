// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationSideEffect.h"

#include "ConversationContext.h"
#include "Utility/Conversation/RecallConversationNodeUtils.h"

//----------------------------------------------------------------------//
// URecallConversationSideEffectNode
//----------------------------------------------------------------------//
void URecallConversationSideEffectNode::CauseSideEffect_Implementation(const FConversationContext& Context) const
{
}

void URecallConversationSideEffectNode::ServerCauseSideEffect_Implementation(const FConversationContext& Context) const
{
	if (Recall::Conversation::Node::Utils::IsBeingRestored(Context))
	{
		return;
	}

	CauseSideEffect_Implementation(Context);
}

void URecallConversationSideEffectNode::ClientCauseSideEffect_Implementation(const FConversationContext& Context) const
{
	// Each client handle server logic, so skip this one.
}
