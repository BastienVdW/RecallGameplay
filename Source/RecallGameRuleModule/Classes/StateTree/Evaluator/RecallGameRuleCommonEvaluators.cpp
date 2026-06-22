// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommonEvaluators.h"

#include "MassEntityView.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "System/Entity/RecallEntitySubsystem.h"
#include "System/Game/RecallGameRuleSubsystem.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// FRecallGameRuleMatchStateEvaluator
//----------------------------------------------------------------------//
bool FRecallGameRuleMatchStateEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameRuleSystemHandle);
	return true;
}

void FRecallGameRuleMatchStateEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallGameRuleMatchStateEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const URecallGameRuleSubsystem& GameRuleSystem = Context.GetExternalData(GameRuleSystemHandle);	
	
	InstanceData.MatchState = GameRuleSystem.GetMatchState();
}

//----------------------------------------------------------------------//
// FRecallGameRuleFilterControllerEvaluator
//----------------------------------------------------------------------//
bool FRecallGameRuleFilterControllerEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySystemHandle);
	return true;
}

void FRecallGameRuleFilterControllerEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallGameRuleFilterControllerEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const URecallEntitySubsystem& EntitySystem = Context.GetExternalData(EntitySystemHandle);
	const TArray<FMassEntityHandle>& ControllerEntities = EntitySystem.GetControllerEntities();
	
	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.TotalControllerCount = ControllerEntities.Num();
	InstanceData.TotalPlayerCount = 0;
	InstanceData.FilteredControllerCount = 0;

	for (const FMassEntityHandle& ControllerEntity : ControllerEntities)
	{
		const FMassEntityView ControllerView(RecallContext.GetEntityManager(), ControllerEntity);

		if (ControllerView.HasTag<FRecallPlayerControllerTag>())
		{
			InstanceData.TotalPlayerCount++;
		}
		else if (InstanceData.bFilterPlayerOnly)
		{
			continue;
		}

		const auto* GameplayTagFragmentPtr = ControllerView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		if (!InstanceData.GameplayTagFilter.IsEmpty() &&
				(GameplayTagFragmentPtr == nullptr ||
					!Recall::GameplayTag::Utils::EvaluateCondition(
						InstanceData.GameplayTagFilter, GameplayTagFragmentPtr->GameplayTagCountMap)))
		{
			continue;
		}

		InstanceData.FilteredControllerCount++;
	}
}
