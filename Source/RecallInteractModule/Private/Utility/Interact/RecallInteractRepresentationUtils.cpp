// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Interact/RecallInteractRepresentationUtils.h"

#include "Components/MeshComponent.h"
#include "Data/Interact/RecallInteractTypes.h"
#include "Interact/RecallInteractReactInterface.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "Utility/Interact/RecallInteractUtils.h"

namespace Recall::Interact::Representation::Utils
{

bool ShouldShowProgress(const FRecallInteractRepresentationContext& Context,
	const FRecallInteractorFragment& InteractorFragment, const FMassExtendedEntityHandle& InteractableEntity)
{
	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	const FMassExtendedEntityView InteractableView(EntityManager, InteractableEntity);
	const auto* InteractableConstSharedFragmentPtr = InteractableView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	const FRecallInteractableFragment& InteractableFragment = InteractableView.GetFragmentData<FRecallInteractableFragment>();
	return InteractableFragment.GetInteractEventChecked(InteractorFragment.InteractEventIndex, InteractableConstSharedFragmentPtr).Duration > 0.0f;
}

FVector GetInteractableLocation(const FRecallInteractRepresentationContext& Context,
	const FMassExtendedEntityHandle& InteractableEntity, const FMassExtendedEntityHandle& InteractorEntity,
	int32 PositionIndex)
{
	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (!EntityManager.IsEntityValid(InteractableEntity))
	{
		return FVector::ZeroVector;
	}

	const FMassExtendedEntityView InteractableView(EntityManager, InteractableEntity);
	const FRecallTransformFragment& InteractableTransformFragment = InteractableView.GetFragmentData<FRecallTransformFragment>();

	const auto* InteractableConstSharedFragmentPtr = InteractableView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	const FRecallInteractableFragment& InteractableFragment = InteractableView.GetFragmentData<FRecallInteractableFragment>();

	// Priority 1: Check for multi-position interaction location (highest priority)
	if (PositionIndex != INDEX_NONE && InteractableFragment.IsValidPositionIndex(PositionIndex))
	{
		const FTransform EntityTransform(InteractableTransformFragment.Rotation, InteractableTransformFragment.Position);
		return InteractableFragment.InteractionPositions[PositionIndex].GetWorldLocation(EntityTransform);
	}

	// Priority 2: Check for socket
	const FName& SocketName = InteractableFragment.GetIconSocketName(InteractableConstSharedFragmentPtr);
	if (!SocketName.IsNone())
	{
		if (const auto* ActorRepresentationFragmentPtr = InteractableView.GetFragmentDataPtr<FRecallActorRepresentationFragment>())
		{
			const TWeakObjectPtr<AActor> Actor = Context.ActorSubsystem.GetActor(ActorRepresentationFragmentPtr->ActorHandle);
			if (Actor.IsValid())
			{
				UMeshComponent* MeshComponent = Cast<UMeshComponent>(Actor->GetComponentByClass(UMeshComponent::StaticClass()));
				if (IsValid(MeshComponent))
				{
					return MeshComponent->GetSocketLocation(SocketName);
				}
			}
		}
	}

	// Priority 3: Default to center position
	return InteractableTransformFragment.Position;
}
	
FRecallInteractionRepresentation GetInteractionRepresentation(const FRecallInteractRepresentationContext& Context,
	const FMassExtendedEntityHandle& InteractorEntity, const FMassExtendedEntityHandle& InteractableEntity, bool bIsContextual)
{
	FRecallInteractionRepresentation InteractionRepresentation;

	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (!EntityManager.IsEntityValid(InteractableEntity))
	{
		return InteractionRepresentation;
	}
		
	const FRecallInteractExecuteContext InteractExecuteContext{ Context.ExecutionContext, InteractorEntity };
		
	for (int32 InputIndex = 0; InputIndex < RECALL_INTERACT_INPUT_COUNT; ++InputIndex)
	{
		const ERecallInteractInput Input = static_cast<ERecallInteractInput>(InputIndex);
			
		FText Text;
		TObjectPtr<UInputAction> UIInputAction;
		const int32 InteractionEventIndex = Recall::Interact::Utils::FindInteractEventIndexByInput(InteractExecuteContext,
			InteractableEntity, Input, bIsContextual, &Text, &UIInputAction);

		if (InteractionEventIndex != INDEX_NONE || !Text.IsEmpty())
		{
			FRecallInteractEventState& State = InteractionRepresentation.EventMap.Add(Input);
			State.InputAction = UIInputAction;
			State.Text = Text;
			State.bFailed = InteractionEventIndex == INDEX_NONE;
		}
	}

	return InteractionRepresentation;
}

FRecallInteractState GetInteractInfo(const FRecallInteractRepresentationContext& Context,
	const FMassExtendedEntityHandle& InteractorEntity, const FRecallInteractorFragment& InteractorFragment)
{
	FRecallInteractState InteractState;

	// Progress
	InteractState.Progress.State = InteractorFragment.CurrentInteractEntity.IsSet() ?
		ERecallInteractStateType::InProgress : ERecallInteractStateType::Idle;
	
	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (EntityManager.IsEntityValid(InteractorFragment.CurrentInteractEntity))
	{
		InteractState.Progress.Progress = InteractorFragment.InteractionProgress;
		InteractState.Progress.bHideProgress = !ShouldShowProgress(Context, InteractorFragment, InteractorFragment.CurrentInteractEntity);
	}

	// Interactable
	InteractState.InteractableLocation = GetInteractableLocation(Context, InteractorFragment.ClosestInteractableEntity, InteractorEntity, InteractorFragment.ClosestPositionIndex);
	InteractState.Interactable = GetInteractionRepresentation(Context,
		InteractorEntity, InteractorFragment.ClosestInteractableEntity, false);
	
	// Contextual
	InteractState.Contextual = GetInteractionRepresentation(Context,
		InteractorEntity, InteractorFragment.ContextualInteractEntity, true);

	return InteractState;
}

}
