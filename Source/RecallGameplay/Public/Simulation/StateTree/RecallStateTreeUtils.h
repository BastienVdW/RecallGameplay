// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "System/AI/RecallStateTreeSubsystem.h"

struct FMassExtendedEntityManager;
class URecallSignalSubsystem;

namespace Recall::StateTree::Behavior
{

template<typename TFunc>
RECALLGAMEPLAY_API inline void ForEachEntityInChunk(
	FMassExtendedExecutionContext& Context,
	FMassExtendedEntityManager& EntityManager,
	URecallSignalSubsystem& SignalSubsystem,
	URecallStateTreeSubsystem& RecallStateTreeSubsystem,
	TFunc&& Callback)
{
	const TArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetMutableFragmentView<FRecallStateTreeInstanceFragment>();
	const FRecallStateTreeSharedFragment& SharedStateTree = Context.GetConstSharedFragment<FRecallStateTreeSharedFragment>();

	// Assuming that all the entities share same StateTree, because they all should have the same storage fragment.
	const int32 NumEntities = Context.GetNumEntities();
	check(NumEntities > 0);
	
	const UStateTree* StateTree = SharedStateTree.StateTree;

	for (int32 EntityIndex = 0; EntityIndex < NumEntities; EntityIndex++)
	{
		const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
		FRecallStateTreeInstanceFragment& StateTreeFragment = StateTreeInstanceList[EntityIndex];
		FStateTreeInstanceData* InstanceData = RecallStateTreeSubsystem.GetInstanceData(StateTreeFragment.InstanceHandle);
		if (InstanceData != nullptr)
		{
			FRecallStateTreeExecutionContext StateTreeContext(RecallStateTreeSubsystem, *StateTree, *InstanceData, EntityManager, SignalSubsystem, Context, Entity);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			// Make sure all required external data are set.
			{
				CSV_SCOPED_TIMING_STAT_EXCLUSIVE(StateTreeProcessorExternalDataValidation);
				if (!ensureMsgf(StateTreeContext.AreContextDataViewsValid(), TEXT("StateTree will not execute due to missing external data.")))
				{
					break;
				}
			}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

			Callback(StateTreeContext, StateTreeFragment);
		}
	}
}

} // Recall::StateTree::Behavior
