// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationEntryPointSideEffect.h"

#include "ConversationContext.h"
#include "MassExtendedEntityView.h"
#include "Simulation/Conversation/RecallConversationFragments.h"
#include "Utility/Conversation/RecallConversationNodeUtils.h"

//----------------------------------------------------------------------//
// URecallConversationEntryPointSideEffect
//----------------------------------------------------------------------//
void URecallConversationEntryPointSideEffect::CauseSideEffect_Implementation(
	const FConversationContext& Context) const
{
	const FMassExtendedEntityHandle OwnerEntity = Recall::Conversation::Node::Utils::GetConversationEntity(Context);
	const FMassExtendedEntityView OwnerView = Recall::Conversation::Node::Utils::CreateEntityView(Context, OwnerEntity);

	FRecallConversationFragment& ConversationFragment = OwnerView.GetFragmentData<FRecallConversationFragment>();
	ConversationFragment.ConversationEntryPoint = ConversationEntryPoint;
}
