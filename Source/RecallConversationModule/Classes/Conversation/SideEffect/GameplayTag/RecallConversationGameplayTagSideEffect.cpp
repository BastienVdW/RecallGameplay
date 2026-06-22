// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationGameplayTagSideEffect.h"

#include "ConversationContext.h"
#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Utility/Conversation/RecallConversationNodeUtils.h"

//----------------------------------------------------------------------//
// URecallConversationGameplayTagSideEffectNode
//----------------------------------------------------------------------//
void URecallConversationGameplayTagSideEffectNode::CauseSideEffect_Implementation(
	const FConversationContext& Context) const
{
	ApplyGameplayTags(Context);
}

static void ApplyGameplayTagOperation(FRecallGameplayTagCountMap& GameplayTagCountMap,
	const FGameplayTagContainer& GameplayTags, ERecallConversationGameplayTagOperation Operation)
{
	switch (Operation)
	{
	case ERecallConversationGameplayTagOperation::Add:
		GameplayTagCountMap.AddTags(GameplayTags);
		break;

	case ERecallConversationGameplayTagOperation::Remove:
		GameplayTagCountMap.RemoveTags(GameplayTags);
		break;

	default:
		unimplemented();
		break;
	}
}

void URecallConversationGameplayTagSideEffectNode::ApplyGameplayTags(const FConversationContext& Context) const
{
	if (bGlobalTags)
	{
		FMassExtendedEntityManager& EntityManager = Recall::Conversation::Node::Utils::GetEntityManagerChecked(
			Context);
		FRecallGameplayTagGlobalSharedFragment& SharedFragment = EntityManager.GetOrCreateSharedFragment<FRecallGameplayTagGlobalSharedFragment>()
			.Get<FRecallGameplayTagGlobalSharedFragment>();
		ApplyGameplayTagOperation(SharedFragment.GameplayTagCountMap, GameplayTags, Operation);
		return;
	}
	
	const TArray<FMassExtendedEntityHandle> Entities = Recall::Conversation::Node::Utils::GetTargetEntities(
		Context, Target);

	for (const FMassExtendedEntityHandle& Entity : Entities)
	{
		const FMassExtendedEntityView EntityView = Recall::Conversation::Node::Utils::CreateEntityView(Context, Entity);
		FRecallGameplayTagFragment* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		if (GameplayTagFragmentPtr == nullptr)
		{
			continue;
		}

		ApplyGameplayTagOperation(GameplayTagFragmentPtr->GameplayTagCountMap, GameplayTags, Operation);
	}	
}
