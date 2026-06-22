// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallEntityTasks.h"

#include "Desync/RecallDesyncLog.h"
#include "MassExtendedCommands.h"
#include "MassExtendedEntityConfigAsset.h"
#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Filter/RecallStateTreeEntityFilterTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Entity/RecallEntitySubsystem.h"

#define LOCTEXT_NAMESPACE "RecallEntityTasks"

//----------------------------------------------------------------------//
// FRecallEntityLocationTask
//----------------------------------------------------------------------//
bool FRecallEntityLocationTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

bool FRecallEntityLocationTask::UpdateEntityLocation(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Entity.IsValid())
	{
		return false;
	}

	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FMassExtendedEntityManager& EntityManager = MassContext.GetEntityManager();
	if (!EntityManager.IsEntityValid(InstanceData.Entity))
	{
		return false;
	}

	const FMassExtendedEntityView EntityView(EntityManager, InstanceData.Entity);

	const FRecallTransformFragment* TransformFragmentPtr = EntityView.GetFragmentDataPtr<FRecallTransformFragment>();
	if (TransformFragmentPtr == nullptr)
	{
		return false;
	}

	FVector* Location = InstanceData.Location.GetMutablePtr(Context);
	if (Location == nullptr)
	{
		return false;
	}

	(*Location) = TransformFragmentPtr->Position;
#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), Location, TransformFragmentPtr->Position);
#endif // RECALL_DESYNC_LOG
	return true;
}

EStateTreeRunStatus FRecallEntityLocationTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (!UpdateEntityLocation(Context))
	{
		return EStateTreeRunStatus::Failed;
	}
	else if (bSucceedOnGet)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FRecallEntityLocationTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (!UpdateEntityLocation(Context))
	{
		return EStateTreeRunStatus::Failed;
	}
	else if (bSucceedOnGet)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

//----------------------------------------------------------------------//
// FRecallGetTaggedEntityTask
//----------------------------------------------------------------------//
bool FRecallFilterEntityTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

EStateTreeRunStatus FRecallFilterEntityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{	
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	TArray<FMassExtendedEntityHandle> Entities = InstanceData.Entities;
	if (Entities.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const TTuple<FMassExtendedEntityHandle*, TArray<FMassExtendedEntityHandle>*> EntityTuple = InstanceData.Result.GetMutablePtrTuple(Context);

	for (const FInstancedStruct& Condition : Conditions)
	{
		const FRecallStateTreeEntityFilterBase* EntityFilterPtr = Condition.GetPtr<FRecallStateTreeEntityFilterBase>();
		if (EntityFilterPtr != nullptr)
		{
			EntityFilterPtr->FilterEntities(MassContext, Entities);
		}
	}
	
	if (Entities.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (EntityTuple.Key)
	{
		(*EntityTuple.Key) = Entities[0];
	}
	else if (EntityTuple.Value)
	{
		(*EntityTuple.Value) = Entities;
	}
	
	if (bSucceedOnFilter)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

//----------------------------------------------------------------------//
// FRecallDestroyEntityTask
//----------------------------------------------------------------------//
bool FRecallDestroyEntityTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

EStateTreeRunStatus FRecallDestroyEntityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{	
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);	
	const TArray<FMassExtendedEntityHandle>& Entities = InstanceData.Entities;
	if (Entities.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	MassContext.GetMassExecutionContext().Defer().DestroyEntities(Entities);

	if (bSucceedOnDestroy)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

//----------------------------------------------------------------------//
// FRecallSpawnEntityTask
//----------------------------------------------------------------------//
bool FRecallSpawnEntityTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySystemHandle);
	return true;
}

EStateTreeRunStatus FRecallSpawnEntityTask::EnterState(FStateTreeExecutionContext& Context,
													  const FStateTreeTransitionResult& Transition) const
{
	URecallEntitySubsystem& EntitySystem = Context.GetExternalData(EntitySystemHandle);
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UMassExtendedEntityConfigAsset* EntityConfigAsset = InstanceData.EntityConfig;
	if (!IsValid(EntityConfigAsset))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const FVector& SpawnPosition = InstanceData.Position;

	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	RecallContext.GetMassExecutionContext().Defer().PushCommand<FMassExtendedDeferredCreateCommand>(
		[&EntitySystem, EntityConfigAsset, SpawnPosition](FMassExtendedEntityManager& System)
	{
		TArray<FMassExtendedEntityHandle> Entities;
		EntitySystem.CreateEntities(EntityConfigAsset, 1, Entities);

		const FMassExtendedEntityView EntityView(System, Entities[0]);
		FRecallTransformFragment& TransformFragment = EntityView.GetFragmentData<FRecallTransformFragment>();
		TransformFragment.Position = SpawnPosition;
	});

	if (bSucceedOnSpawn)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

void FRecallSpawnEntityTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallSpawnEntityTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return Super::Tick(Context, DeltaTime);
}
