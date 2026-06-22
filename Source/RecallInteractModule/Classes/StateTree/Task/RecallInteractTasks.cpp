// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractTasks.h"

#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Interact/RecallInteractPositionUtils.h"
#include "Utility/Interact/RecallInteractUtils.h"

//----------------------------------------------------------------------//
// FRecallInteractTask
//----------------------------------------------------------------------//
FRecallInteractTask::FRecallInteractTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallInteractTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(InteractorFragmentHandle);
	Linker.LinkExternalData(GameplayTagFragmentHandle);
	Linker.LinkExternalData(AttributeFragmentHandle);
	return true;
}

FRecallInteractExecuteContext FRecallInteractTask::CreateInteractionExecutionContext(FStateTreeExecutionContext& Context) const
{
	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	
	FRecallInteractorFragment& InteractorFragment = Context.GetExternalData(InteractorFragmentHandle);
	FRecallGameplayTagFragment* const GameplayTagFragmentPtr = Context.GetExternalDataPtr(GameplayTagFragmentHandle);
	FRecallAttributeFragment* const AttributeFragmentPtr = Context.GetExternalDataPtr(AttributeFragmentHandle);
	
	return FRecallInteractExecuteContext{
		RecallContext.GetMassExecutionContext(),
		RecallContext.GetEntity(),
		&InteractorFragment,
		GameplayTagFragmentPtr,
		AttributeFragmentPtr,
		&RecallContext.GetSignalSystem()
	};
}

float FRecallInteractTask::GetInteractionProgressTickRate(FStateTreeExecutionContext& Context) const
{
	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FMassExtendedEntityManager& EntityManager = RecallContext.GetEntityManager();
	FRecallInteractorFragment& InteractorFragment = Context.GetExternalData(InteractorFragmentHandle);
	
	const FMassExtendedEntityView CurrentInteractView(EntityManager, InteractorFragment.CurrentInteractEntity);
	const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	const FRecallInteractableFragment& InteractableFragment = CurrentInteractView.GetFragmentData<FRecallInteractableFragment>();
	const FRecallInteractionEvent& InteractionEvent = InteractableFragment.GetInteractEventChecked(
		InteractorFragment.InteractEventIndex, InteractableConstSharedFragmentPtr);
	return InteractionEvent.ProgressTickRate;
}

EStateTreeRunStatus FRecallInteractTask::EnterState(FStateTreeExecutionContext& Context,
                                                      const FStateTreeTransitionResult& Transition) const
{	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bExitOnRelease = true; // Reset the flag
	
	const FRecallInteractExecuteContext InteractContext = CreateInteractionExecutionContext(Context);
	const FMassExtendedEntityHandle InteractionTarget = GetInteractSelection(Context);
	
	const bool bContextual = IsContextualInteraction(Context);
	const int32 InteractEventIndex = Recall::Interact::Utils::FindInteractEventIndexByInput(
		InteractContext, InteractionTarget, InstanceData.Input, bContextual);	
	if (InteractEventIndex == INDEX_NONE)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (!Recall::Interact::Utils::BeginInteract(InteractContext, InteractionTarget, InteractEventIndex))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const float ProgressTickRate = GetInteractionProgressTickRate(Context);
	if (ProgressTickRate > 0.0f)
	{
		FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
		RecallContext.GetSignalSystem().DelaySignalEntity(Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity(), ProgressTickRate);
	}

	// Wait until the first update
	InstanceData.bExitOnRelease = false;
	
	return Super::EnterState(Context, Transition);
}

void FRecallInteractTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FRecallInteractExecuteContext InteractContext = CreateInteractionExecutionContext(Context);
	
	Recall::Interact::Utils::EndInteract(InteractContext);
	
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallInteractTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FRecallInteractExecuteContext InteractContext = CreateInteractionExecutionContext(Context);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const bool bContextual = IsContextualInteraction(Context);
	const ERecallInteractResult InteractResult = Recall::Interact::Utils::UpdateInteract(
		InteractContext, InstanceData.Input, DeltaTime, InstanceData.bExitOnRelease, bContextual);	
	if (InteractResult == ERecallInteractResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}
	else if (InteractResult == ERecallInteractResult::Succeeded)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	else if (InteractResult == ERecallInteractResult::Running)
	{
		const float ProgressTickRate = GetInteractionProgressTickRate(Context);
		if (ProgressTickRate > 0.0f)
		{
			FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
			RecallContext.GetSignalSystem().DelaySignalEntity(
				Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity(), ProgressTickRate);
		}
		
		return Super::Tick(Context, DeltaTime);
	}
	else
	{
		unimplemented();
		return EStateTreeRunStatus::Failed;
	}
}

FMassExtendedEntityHandle FRecallInteractTask::GetInteractSelection(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallInteractorFragment& InteractorFragment = Context.GetExternalData(InteractorFragmentHandle);

	switch (InstanceData.Selection)
	{
	case ERecallInteractSelection::Closest:
		return InteractorFragment.ClosestInteractableEntity;

	case ERecallInteractSelection::Target:
		if (const FMassExtendedEntityHandle* TargetPtr = InstanceData.Target.GetMutablePtr(Context))
		{
			return *TargetPtr;
		}
		else
		{
			return FMassExtendedEntityHandle();
		}
		
	case ERecallInteractSelection::Contextual:
		return InteractorFragment.ContextualInteractEntity;

	default:
		unimplemented();
		return FMassExtendedEntityHandle();
	}
}

bool FRecallInteractTask::IsContextualInteraction(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	return InstanceData.Selection == ERecallInteractSelection::Contextual;
}

//----------------------------------------------------------------------//
// FRecallFindInteractableTask
//----------------------------------------------------------------------//

bool FRecallFindInteractableTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(InteractorFragmentHandle);
	Linker.LinkExternalData(GameplayTagFragmentHandle);
	Linker.LinkExternalData(AttributeFragmentHandle);
	Linker.LinkExternalData(TransformFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallFindInteractableTask::EnterState(FStateTreeExecutionContext& Context,
                                                               const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallStateTreeExecutionContext& RecallContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);

	// Get required fragments
	FRecallInteractorFragment& InteractorFragment = Context.GetExternalData(InteractorFragmentHandle);
	FRecallGameplayTagFragment* const GameplayTagFragmentPtr = Context.GetExternalDataPtr(GameplayTagFragmentHandle);
	FRecallAttributeFragment* const AttributeFragmentPtr = Context.GetExternalDataPtr(AttributeFragmentHandle);
	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);

	const FMassExtendedEntityHandle& InteractorEntity = RecallContext.GetEntity();
	const FMassExtendedEntityManager& EntityManager = RecallContext.GetMassExecutionContext().GetEntityManagerChecked();

	// Create interaction context for validation
	const FRecallInteractExecuteContext InteractContext{
		RecallContext.GetMassExecutionContext(),
		InteractorEntity,
		&InteractorFragment,
		GameplayTagFragmentPtr,
		AttributeFragmentPtr,
		&RecallContext.GetSignalSystem()
	};

	// Track the best candidate
	FMassExtendedEntityHandle BestEntity;
	int32 BestPositionIndex = INDEX_NONE;
	FVector BestInteractLocation = FVector::ZeroVector;
	float BestDistanceSquared = FLT_MAX;

	const FVector InteractorLocation = TransformFragment.GetTransform().GetLocation();

	// Iterate through all candidate entities
	for (const FMassExtendedEntityHandle& CandidateEntity : InstanceData.Entities)
	{
		// Skip invalid entities
		if (!EntityManager.IsEntityValid(CandidateEntity))
		{
			continue;
		}

		// Check if this entity can be interacted with using the existing interact utility
		const int32 InteractEventIndex = Recall::Interact::Utils::FindInteractEventIndexByInput(
			InteractContext, CandidateEntity, InstanceData.Input, false);

		// Skip if not interactable
		if (InteractEventIndex == INDEX_NONE)
		{
			continue;
		}

		// Get the interactable view
		const FMassExtendedEntityView InteractableView(EntityManager, CandidateEntity);

		// Determine the interaction position and calculate distance
		int32 PositionIndex = INDEX_NONE;
		FVector InteractLocation = FVector::ZeroVector;
		float DistanceSquared = 0.0f;

		if (!Recall::Interact::Position::Utils::CalculateInteractionLocationAndDistance(
			InteractorLocation, CandidateEntity, InteractableView, EntityManager, PositionIndex, InteractLocation, DistanceSquared))
		{
			// Skip if calculation failed (no available positions, missing fragments, etc.)
			continue;
		}

		// Update best candidate if this is closer
		if (DistanceSquared < BestDistanceSquared)
		{
			BestEntity = CandidateEntity;
			BestPositionIndex = PositionIndex;
			BestInteractLocation = InteractLocation;
			BestDistanceSquared = DistanceSquared;
		}
	}

	// Output results
	InstanceData.BestInteractable = BestEntity;
	InstanceData.NearestPositionIndex = BestPositionIndex;
	InstanceData.InteractLocation = BestInteractLocation;

	// Return success if we found a valid entity, failure otherwise
	if (BestEntity.IsValid() && bSucceedOnFind)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	else if (BestEntity.IsValid())
	{
		return EStateTreeRunStatus::Running;
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}
}
