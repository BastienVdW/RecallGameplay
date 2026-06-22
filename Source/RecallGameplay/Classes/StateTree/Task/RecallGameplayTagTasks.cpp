// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayTagTasks.h"

#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityTypes.h"
#include "MassExtendedEntityView.h"
#include "RecallSignalSubsystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "Utility/Entity/RecallEntityUtils.h"

//----------------------------------------------------------------------//
// FRecallGetTaggedEntityTask
//----------------------------------------------------------------------//
bool FRecallGetTaggedEntityTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

EStateTreeRunStatus FRecallGetTaggedEntityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{	
	const TArray<FMassExtendedEntityHandle> SourceEntities = GetEntities(Context);
	if (SourceEntities.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const TTuple<FMassExtendedEntityHandle*, TArray<FMassExtendedEntityHandle>*> EntityTuple = InstanceData.Entity.GetMutablePtrTuple(Context);
	const int32 EntityCount = EntityTuple.Key ? 1 : INT32_MAX;
	const TArray<FMassExtendedEntityHandle> ResultEntities = Recall::Entity::Utils::FilterEntitiesByTag(
		Context.GetWorld(), SourceEntities, InstanceData.RequiredGameplayTags, RequiredNameTags, EntityCount);
	if (ResultEntities.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (EntityTuple.Key)
	{
		(*EntityTuple.Key) = ResultEntities[0];
	}
	else if (EntityTuple.Value)
	{
		(*EntityTuple.Value) = ResultEntities;
	}
	
	if (bSucceedOnGet)
	{
		FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
		MassContext.GetSignalSystem().SignalEntity(
			Recall::StateTree::Signals::TickRequired, MassContext.GetEntity());

		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

TArray<FMassExtendedEntityHandle> FRecallGetTaggedEntityTask::GetEntities(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	switch (InstanceData.Source)
	{
	case ERecallGetTaggedEntityTaskSource::AllMutable:
		return Recall::Entity::Utils::GetAllMutableEntities(Context.GetWorld());

	case ERecallGetTaggedEntityTaskSource::Selection:
		return InstanceData.SelectedEntities;

	default:
		unimplemented();
		return {};
	}
}

//----------------------------------------------------------------------//
// FRecallAddGameplayTagTask
//----------------------------------------------------------------------//
FRecallAddGameplayTagTask::FRecallAddGameplayTagTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallAddGameplayTagTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameplayTagFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallAddGameplayTagTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (FRecallGameplayTagFragment* GameplayTagFragmentPtr = GetTargetGameplayTagFragment(Context))
	{
		GameplayTagFragmentPtr->GameplayTagCountMap.AddTags(GameplayTags);
	}

	return Super::EnterState(Context, Transition);
}

void FRecallAddGameplayTagTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (FRecallGameplayTagFragment* GameplayTagFragmentPtr = GetTargetGameplayTagFragment(Context))
	{
		GameplayTagFragmentPtr->GameplayTagCountMap.RemoveTags(GameplayTags);
	}
	
	return Super::ExitState(Context, Transition);
}

FRecallGameplayTagFragment* FRecallAddGameplayTagTask::GetTargetGameplayTagFragment(
	FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	switch (InstanceData.Target)
	{
	case ERecallAddGameplayTagTarget::Self:
		return &Context.GetExternalData(GameplayTagFragmentHandle);

	case ERecallAddGameplayTagTarget::Entity:
		{
			const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
			const FMassExtendedEntityManager& EntityManager = MassContext.GetEntityManager();
			if (EntityManager.IsEntityValid(InstanceData.Entity))
			{
				const FMassExtendedEntityView EntityView(MassContext.GetEntityManager(), InstanceData.Entity);
				return EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
			}	
		}
		break;

	default:
		unimplemented();
		return nullptr;
	}
	
	return nullptr;
}
