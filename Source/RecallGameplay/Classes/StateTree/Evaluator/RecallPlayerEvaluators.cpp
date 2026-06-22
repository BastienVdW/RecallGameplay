// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPlayerEvaluators.h"

#include "Data/Input/RecallInputActionTableRow.h"
#include "MassEntityView.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Player/Input/RecallPlayerInputFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Entity/RecallEntitySubsystem.h"

//----------------------------------------------------------------------//
// FRecallPlayerLocationEvaluator
//----------------------------------------------------------------------//
bool FRecallPlayerLocationEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySystemHandle);
	return true;
}

void FRecallPlayerLocationEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallPlayerLocationEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.bFoundPlayer = false;
	InstanceData.PlayerLocation = FVector::ZeroVector;
	InstanceData.PlayerEntity.Reset();

	const URecallEntitySubsystem& EntitySystem = Context.GetExternalData(EntitySystemHandle);
	const TArray<FString> PlayerIds = EntitySystem.GetControllerIDs();

	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);

	for (const FString& PlayerId : PlayerIds)
	{
		FMassEntityHandle PlayerEntity;
		if (!ensureMsgf(EntitySystem.FindControllerOwnedEntity(PlayerId, PlayerEntity), TEXT("No player entity")))
		{
			continue;
		}

		const FMassEntityView PlayerView(RecallContext.GetEntityManager(), PlayerEntity);
		if (!PlayerView.HasTag<FRecallPlayerControllerTag>())
		{
			continue;
		}
		
		const FRecallTransformFragment* TransformFragmentPtr = PlayerView.GetFragmentDataPtr<FRecallTransformFragment>();
		if (!ensureMsgf(TransformFragmentPtr, TEXT("Player does not have a transform")))
		{
			continue;
		}

		InstanceData.bFoundPlayer = true;
		InstanceData.PlayerLocation = TransformFragmentPtr->Position;
		InstanceData.PlayerEntity = PlayerEntity;
		break;
	}
}

//----------------------------------------------------------------------//
// FRecallPlayerAxisEvaluator
//----------------------------------------------------------------------//
bool FRecallPlayerAxisEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(InputFragmentHandle);
	return true;
}

void FRecallPlayerAxisEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallPlayerAxisEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallPlayerInputFragment& InputFragment = Context.GetExternalData(InputFragmentHandle);
	
	const ERecallControllerInputCommand ControllerInputCommand = FRecallInputActionTableRow::GetControllerInputCommandByAction(InstanceData.InputAxis);
	InstanceData.Value = InputFragment.GetAxis1D(ControllerInputCommand);
}
