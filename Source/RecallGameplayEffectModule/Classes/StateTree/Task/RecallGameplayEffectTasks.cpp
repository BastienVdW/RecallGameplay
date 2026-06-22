// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectTasks.h"

#include "Gameplay/RecallGameplayEffect.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/GameplayEffect/RecallGameplayEffectFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"

//----------------------------------------------------------------------//
// FRecallAddGameplayEffectTask
//----------------------------------------------------------------------//
FRecallAddGameplayEffectTask::FRecallAddGameplayEffectTask()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallAddGameplayEffectTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameplayEffectFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallAddGameplayEffectTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (!Effect)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	FRecallGameplayEffectFragment& GameplayEffectFragment = Context.GetExternalData(GameplayEffectFragmentHandle);

	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRecallGameplayEffectContext GameplayEffectContext {
		RecallContext.GetEntityManager(), RecallContext.GetEntity()
	};
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.EffectHandle = GameplayEffectFragment.Container.AddEffect(GameplayEffectContext, Effect);

	if (bSucceedOnAdd)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

void FRecallAddGameplayEffectTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (bSucceedOnAdd || !bRemoveOnExit)
	{
		return;
	}
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.EffectHandle.IsSet())
	{
		return;
	}

	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRecallGameplayEffectContext GameplayEffectContext {
		RecallContext.GetEntityManager(), RecallContext.GetEntity()
	};
		
	FRecallGameplayEffectFragment& GameplayEffectFragment = Context.GetExternalData(GameplayEffectFragmentHandle);
	GameplayEffectFragment.Container.RemoveEffect(GameplayEffectContext, InstanceData.EffectHandle);
}

//----------------------------------------------------------------------//
// FRecallRemoveGameplayEffectTask
//----------------------------------------------------------------------//
FRecallRemoveGameplayEffectTask::FRecallRemoveGameplayEffectTask()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallRemoveGameplayEffectTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameplayEffectFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallRemoveGameplayEffectTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FRecallGameplayEffectFragment& GameplayEffectFragment = Context.GetExternalData(GameplayEffectFragmentHandle);

	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRecallGameplayEffectContext GameplayEffectContext {
		RecallContext.GetEntityManager(), RecallContext.GetEntity()
	};
	
	GameplayEffectFragment.Container.RemoveEffect(GameplayEffectContext, InstanceData.EffectHandle);
	GameplayEffectFragment.Container.RemoveEffectsByClass(GameplayEffectContext, Effect);

	if (bSucceedOnRemove)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}
