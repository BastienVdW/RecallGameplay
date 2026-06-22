// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallTokenTasks.h"

#include "MassEntityManager.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/StateTree/RecallStateTreeTokenFragments.h"

//----------------------------------------------------------------------//
// FRecallTokenTask
//----------------------------------------------------------------------//
FRecallTokenTask::FRecallTokenTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallTokenTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TokenSharedFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallTokenTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FRecallStateTreeExecutionContext& MassContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!MassContext.GetEntityManager().IsEntityValid(InstanceData.TargetEntity))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	FRecallStateTreeTokenSharedFragment& TokenSharedFragment = Context.GetExternalData(TokenSharedFragmentHandle);
	if (TokenSharedFragment.GetTokenCount(InstanceData.TargetEntity) >= InstanceData.TokenLimit)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	InstanceData.TokenHandle = TokenSharedFragment.RequestToken(InstanceData.TargetEntity);
	
	return Super::EnterState(Context, Transition);
}

void FRecallTokenTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	FRecallStateTreeTokenSharedFragment& TokenSharedFragment = Context.GetExternalData(TokenSharedFragmentHandle);
	TokenSharedFragment.ReleaseToken(InstanceData.TokenHandle);

	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallTokenTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FRecallStateTreeExecutionContext& MassContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Invalidate token if entity is not valid
	if (!MassContext.GetEntityManager().IsEntityValid(InstanceData.TokenHandle.GetTokenOwnerEntity()))
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::Tick(Context, DeltaTime);
}
