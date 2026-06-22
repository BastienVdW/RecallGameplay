// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCommonEvaluators.h"

#include "Data/Faction/RecallFactionTypes.h"
#include "Desync/RecallDesyncLog.h"
#include "MassExtendedEntityView.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/AI/RecallStateTreeSubsystem.h"

//----------------------------------------------------------------------//
// FRecallTowardLocationEvaluator
//----------------------------------------------------------------------//
bool FRecallTowardLocationEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformFragmentHandle);
	return true;
}

void FRecallTowardLocationEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallTowardLocationEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);

	FVector Direction(InstanceData.Direction.X, InstanceData.Direction.Y, 0.0);

	if (InstanceData.bNormalize)
	{
		Direction.Normalize();
	}

	if (!InstanceData.bAllowZero && Direction.IsNearlyZero())
	{
		Direction.X = InstanceData.DefaultZeroValue.X;
		Direction.Y = InstanceData.DefaultZeroValue.Y;
	}

	InstanceData.Location = TransformFragment.Position + Direction * AheadDistance;

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), TowardLocation, InstanceData.Location);
#endif // RECALL_DESYNC_LOG
}

//----------------------------------------------------------------------//
// FRecallForwardVectorEvaluator
//----------------------------------------------------------------------//
bool FRecallForwardVectorEvaluator::Link(FStateTreeLinker& Linker)
{
	return true;
}

void FRecallForwardVectorEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallForwardVectorEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Vector = InstanceData.Rotation.GetForwardVector();

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), ForwardVector, InstanceData.Vector);
#endif // RECALL_DESYNC_LOG
}

//----------------------------------------------------------------------//
// FRecallEventPayloadEvaluator
//----------------------------------------------------------------------//
template<typename T>
static void AddExtractedData(TArray<T>& Array, const T& Data, ERecallEventPayloadArrayOperation ArrayOperation)
{
	switch (ArrayOperation)
	{
	case ERecallEventPayloadArrayOperation::AddUnique:
		Array.AddUnique(Data);
		break;

	case ERecallEventPayloadArrayOperation::Add:
		Array.Add(Data);
		break;

	case ERecallEventPayloadArrayOperation::Remove:
		Array.Remove(Data);
		break;

	default:
		unimplemented();
		break;
	}
}

template<typename T>
static void ExtractPropertyFromPayloads(const TArray<FInstancedStruct>& Payloads, const TTuple<T*, TArray<T>*>& Result, ERecallEventPayloadArrayOperation ArrayOperation)
{
	for (const FInstancedStruct& DataItem : Payloads)
	{
		const T* DataPtr = DataItem.GetPtr<T>();
		if (!ensureMsgf(DataPtr != nullptr, TEXT("Data type does not match")))
		{
			continue;
		}

		if (Result.Key)
		{
			(*Result.Key) = *DataPtr;
			break;
		}
		else if (Result.Value)
		{
			AddExtractedData(*Result.Value, *DataPtr, ArrayOperation);
		}
	}
}

bool FRecallEventPayloadEvaluator::Link(FStateTreeLinker& Linker)
{
	return true;
}

void FRecallEventPayloadEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallEventPayloadEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!Tag.IsValid())
	{
		return;
	}

	InstanceData.Payloads.Reset();

	for (const FStateTreeSharedEvent& EventPtr : Context.GetEventsToProcessView())
	{
		if (!EventPtr.IsValid() || EventPtr->Tag != Tag || !EventPtr->Payload.IsValid())
		{
			continue;
		}

		InstanceData.Payloads.Add(EventPtr->Payload);
	}

	if (InstanceData.Payloads.IsEmpty())
	{
		return;
	}

	switch (InstanceData.Type)
	{
	case ERecallEventPayloadType::Vector:
		ExtractPropertyFromPayloads(InstanceData.Payloads, InstanceData.VectorResult.GetMutablePtrTuple(Context), ArrayOperation);
		break;

	case ERecallEventPayloadType::Entity:
		ExtractPropertyFromPayloads(InstanceData.Payloads, InstanceData.EntityResult.GetMutablePtrTuple(Context), ArrayOperation);
		break;

	default:
		unimplemented();
		break;
	}
}

//----------------------------------------------------------------------//
// FRecallStateTreeEvaluator
//----------------------------------------------------------------------//
bool FRecallStateTreeEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(StateTreeInstanceFragmentHandle);
	Linker.LinkExternalData(StateTreeSystemHandle);
	return true;
}

void FRecallStateTreeEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
	if (!ensureMsgf(StateTree, TEXT("State tree must be set")))
	{
		return;
	}

	URecallStateTreeSubsystem& StateTreeSystem = Context.GetExternalData(StateTreeSystemHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!ensureMsgf(!InstanceData.InstanceHandle.IsValid(), TEXT("State tree is already running (somehow)")))
	{
		StateTreeSystem.FreeInstanceData(InstanceData.InstanceHandle);
	}

	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	FRecallStateTreeInstanceFragment& StateTreeInstanceFragment = Context.GetExternalData(StateTreeInstanceFragmentHandle);

	const int32 RandomSeed = MassContext.GetRandomStream().RandRange(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Max());

	InstanceData.InstanceHandle = StateTreeSystem.AllocateInstanceData();
	if (!ensureMsgf(InstanceData.InstanceHandle.IsValid(), TEXT("Failed to create state tree instance data")))
	{
		return;
	}

	StateTreeInstanceFragment.SubInstanceHandles.Add(InstanceData.InstanceHandle);

	if (!StateTreeName.IsNone() && ensureMsgf(!StateTreeInstanceFragment.SubInstanceHandleMap.Contains(StateTreeName), TEXT("Already a state tree named: %s"), *StateTreeName.ToString()))
	{
		StateTreeInstanceFragment.SubInstanceHandleMap.Add(StateTreeName, InstanceData.InstanceHandle);
	}

	URecallSignalSubsystem& SignalSystem =  MassContext.GetSignalSystem();
	FStateTreeInstanceData* StateTreeInstanceDataPtr = StateTreeSystem.GetInstanceData(InstanceData.InstanceHandle);

	FRecallStateTreeExecutionContext StateTreeContext(StateTreeSystem, *StateTree, *StateTreeInstanceDataPtr, MassContext.GetEntityManager(), SignalSystem, MassContext.GetMassExecutionContext(), MassContext.GetEntity());
	StateTreeContext.Start(nullptr, RandomSeed);

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_INT(Context.GetWorld(), StateTreeRandomSeed, RandomSeed)
#endif // RECALL_DESYNC_LOG
}

void FRecallStateTreeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.InstanceHandle.IsValid())
	{
		return;
	}

	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	URecallStateTreeSubsystem& StateTreeSystem = Context.GetExternalData(StateTreeSystemHandle);
	URecallSignalSubsystem& SignalSystem =  MassContext.GetSignalSystem();
	FStateTreeInstanceData* StateTreeInstanceDataPtr = StateTreeSystem.GetInstanceData(InstanceData.InstanceHandle);

	FRecallStateTreeExecutionContext StateTreeContext(StateTreeSystem, *StateTree, *StateTreeInstanceDataPtr, MassContext.GetEntityManager(), SignalSystem, MassContext.GetMassExecutionContext(), MassContext.GetEntity());

	// Tick the tree instance
	StateTreeContext.Tick(DeltaTime);

	// When last tick status is different than "Running", the state tree need to be tick again
	// For performance reason, tick again to see if we could find a new state right away instead of waiting the next frame.
	if (StateTreeContext.GetLastTickStatus() != EStateTreeRunStatus::Running)
	{
		StateTreeContext.Tick(0.0f);
	}
}

void FRecallStateTreeEvaluator::TreeStop(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.InstanceHandle.IsValid())
	{
		return;
	}

	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	URecallStateTreeSubsystem& StateTreeSystem = Context.GetExternalData(StateTreeSystemHandle);
	FRecallStateTreeInstanceFragment& StateTreeInstanceFragment = Context.GetExternalData(StateTreeInstanceFragmentHandle);

	URecallSignalSubsystem& SignalSystem =  MassContext.GetSignalSystem();
	FStateTreeInstanceData* StateTreeInstanceDataPtr = StateTreeSystem.GetInstanceData(InstanceData.InstanceHandle);

	FRecallStateTreeExecutionContext StateTreeContext(StateTreeSystem, *StateTree, *StateTreeInstanceDataPtr, MassContext.GetEntityManager(), SignalSystem, MassContext.GetMassExecutionContext(), MassContext.GetEntity());
	StateTreeContext.Stop();

	if (!StateTreeName.IsNone() && ensureMsgf(StateTreeInstanceFragment.SubInstanceHandleMap.Contains(StateTreeName), TEXT("Named state tree was already removed: %s"), *StateTreeName.ToString()))
	{
		StateTreeInstanceFragment.SubInstanceHandleMap.Remove(StateTreeName);
	}

	StateTreeInstanceFragment.SubInstanceHandles.Remove(InstanceData.InstanceHandle);
	StateTreeSystem.FreeInstanceData(InstanceData.InstanceHandle);
}
