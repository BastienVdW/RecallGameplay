// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationGameplayTagRequirement.h"

#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Utility/Conversation/RecallConversationNodeUtils.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// URecallConversationGameplayTagRequirementNode
//----------------------------------------------------------------------//
EConversationRequirementResult URecallConversationGameplayTagRequirementNode::IsRequirementSatisfied_Implementation(
	const FConversationContext& Context) const
{
	if (Recall::Conversation::Node::Utils::IsBeingRestored(Context))
	{
		return Super::IsRequirementSatisfied_Implementation(Context);
	}
	
	if (EvaluateGameplayTagCondition(Context) != bInvert)
	{
		return RequirementSatisfiedResult;
	}
	else
	{
		return RequirementNotSatisfiedResult;
	}
}

bool URecallConversationGameplayTagRequirementNode::EvaluateGameplayTagCondition(
	const FConversationContext& Context) const
{
	if (bGlobalTags)
	{
		FMassExtendedEntityManager& EntityManager = Recall::Conversation::Node::Utils::GetEntityManagerChecked(
			Context);
		const FRecallGameplayTagGlobalSharedFragment& SharedFragment = EntityManager.GetOrCreateSharedFragment<FRecallGameplayTagGlobalSharedFragment>()
			.Get<FRecallGameplayTagGlobalSharedFragment>();
		return Recall::GameplayTag::Utils::EvaluateCondition(GameplayTagCondition, SharedFragment.GameplayTagCountMap);
	}
	
	const TArray<FMassExtendedEntityHandle> Entities = Recall::Conversation::Node::Utils::GetTargetEntities(Context, Target);

	for (const FMassExtendedEntityHandle& Entity : Entities)
	{
		const FMassExtendedEntityView EntityView = Recall::Conversation::Node::Utils::CreateEntityView(Context, Entity);
		const FRecallGameplayTagFragment* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		if (GameplayTagFragmentPtr == nullptr)
		{
			continue;
		}
		
		if (Recall::GameplayTag::Utils::EvaluateCondition(GameplayTagCondition, GameplayTagFragmentPtr->GameplayTagCountMap))
		{
			return true;
		}
	}

	return false;
}
