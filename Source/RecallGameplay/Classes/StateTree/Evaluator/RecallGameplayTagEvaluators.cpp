// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayTagEvaluators.h"

#include "GameplayTagsManager.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// FRecallFactionEvaluator
//----------------------------------------------------------------------//
bool FRecallFactionEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameplayTagFragmentHandle);
	return true;
}

void FRecallFactionEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallFactionEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const FGameplayTag FactionTag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Faction"));
	InstanceData.EnemyFactionTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(FactionTag);

	const FRecallGameplayTagFragment* GameplayTagFragmentPtr = Context.GetExternalDataPtr(GameplayTagFragmentHandle);
	if (GameplayTagFragmentPtr == nullptr)
	{
		return;
	}

	InstanceData.FactionTags = Recall::GameplayTag::Utils::GetFactionTags(GameplayTagFragmentPtr->GameplayTagCountMap);
	InstanceData.EnemyFactionTags.RemoveTags(InstanceData.FactionTags);
}
