// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationRequirement.h"

#include "Utility/Conversation/RecallConversationNodeUtils.h"

//----------------------------------------------------------------------//
// URecallConversationRequirementNode
//----------------------------------------------------------------------//
EConversationRequirementResult URecallConversationRequirementNode::IsRequirementSatisfied_Implementation(const FConversationContext& Context) const
{
	if (Recall::Conversation::Node::Utils::IsBeingRestored(Context))
	{
		return EConversationRequirementResult::Passed;
	}
	else
	{
		return EConversationRequirementResult::FailedAndHidden;
	}
}
