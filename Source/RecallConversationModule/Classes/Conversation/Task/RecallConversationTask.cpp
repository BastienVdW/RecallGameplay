// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationTask.h"

#include "ConversationContext.h"

//----------------------------------------------------------------------//
// URecallConversationTaskNode
//----------------------------------------------------------------------//
FConversationTaskResult URecallConversationTaskNode::ExecuteTaskNode_Implementation(const FConversationContext& Context) const
{
	return Super::ExecuteTaskNode_Implementation(Context);
}
