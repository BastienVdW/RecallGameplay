// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallParameterTasks.h"

#include "StateTree/RecallStateTreeTypes.h"

template<typename T, typename FInstanceDataType>
static bool SetParameter(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData)
{
	T* ParameterPtr = InstanceData.Parameter.GetMutablePtr(Context);
	if (!ensureMsgf(ParameterPtr != nullptr,
		TEXT("%hs Entity is not set"), __FUNCTION__))
	{
		return false;
	}
	
	switch (InstanceData.Source)
	{
	case ERecallSetParameterSource::Data:
		(*ParameterPtr) = InstanceData.Value;
		return true;

	case ERecallSetParameterSource::Payload:
		if (const T* PayloadParameterPtr = InstanceData.PayloadValue.GetPtr<T>())
		{
			(*ParameterPtr) = (*PayloadParameterPtr);
			return true;
		}
		else
		{
			UE_LOG(LogRecallStateTree, Warning,
				TEXT("%hs No parameter could be found in the payload"), __FUNCTION__);
		}
		break;

	default:
		unimplemented();
		break;
	}
	
	return false;
}

//----------------------------------------------------------------------//
// FRecallSetEntityParameterTask
//----------------------------------------------------------------------//
FRecallSetEntityParameterTask::FRecallSetEntityParameterTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallSetEntityParameterTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

EStateTreeRunStatus FRecallSetEntityParameterTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!SetParameter<FMassEntityHandle>(Context, InstanceData))
	{
		return EStateTreeRunStatus::Failed;
	}
	else if (bSucceedOnSet)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::EnterState(Context, Transition);
}

//----------------------------------------------------------------------//
// FRecallSetVectorParameterTask
//----------------------------------------------------------------------//
FRecallSetVectorParameterTask::FRecallSetVectorParameterTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallSetVectorParameterTask::Link(FStateTreeLinker& Linker)
{
	return true;
}

EStateTreeRunStatus FRecallSetVectorParameterTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!SetParameter<FVector>(Context, InstanceData))
	{
		return EStateTreeRunStatus::Failed;
	}
	else if (bSucceedOnSet)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::EnterState(Context, Transition);
}
