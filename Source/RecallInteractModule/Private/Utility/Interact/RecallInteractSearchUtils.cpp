// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Interact/RecallInteractSearchUtils.h"

#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Interact/RecallInteractPositionUtils.h"
#include "Utility/Interact/RecallInteractUtils.h"

namespace Recall::Interact::Search::Utils
{

/**
 * Calculate interaction distance and validate range for an interactable entity.
 * Uses the same location calculation as the actual interaction system to ensure consistency.
 * @param Context The interaction context
 * @param InteractableFragment The interactable fragment
 * @param OverlappingView The overlapping entity view
 * @param InteractableEntity The interactable entity handle
 * @param InteractEventIndex The interaction event index (-1 if invalid)
 * @param OutSqrDistance Output squared distance
 * @return true if entity is within interaction range, false otherwise
 */
static bool CalculateDistanceAndValidateRange(
	const FRecallUpdateClosestInteractableEntityContext& Context,
	const FRecallInteractableFragment& InteractableFragment,
	const FMassExtendedEntityView& OverlappingView,
	const FMassExtendedEntityHandle& InteractableEntity,
	int32 InteractEventIndex,
	float& OutSqrDistance)
{
	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();

	if (InteractEventIndex != INDEX_NONE)
	{
		const auto* InteractableConstSharedFragmentPtr = OverlappingView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
		const FRecallInteractionEvent& Event = InteractableFragment.GetInteractEventChecked(InteractEventIndex, InteractableConstSharedFragmentPtr);

		// Use the same location calculation as the actual interaction system
		int32 PositionIndex = INDEX_NONE;
		FVector InteractLocation = FVector::ZeroVector;
		if (!Recall::Interact::Position::Utils::CalculateInteractionLocationAndDistance(
			Context.TransformFragment.Position,
			InteractableEntity,
			OverlappingView,
			EntityManager,
			PositionIndex,
			InteractLocation,
			OutSqrDistance))
		{
			return false;
		}

		// Check if interaction is within range (0.0 means unlimited range)
		if (Event.MaxInteractionRange > 0.0f)
		{
			const float MaxSqrDistance = Event.MaxInteractionRange * Event.MaxInteractionRange;
			return OutSqrDistance <= MaxSqrDistance;
		}
	}
	else
	{
		// Fallback to center-to-center distance for failed events
		const FRecallTransformFragment& OverlappingTransformFragment = OverlappingView.GetFragmentData<FRecallTransformFragment>();
		OutSqrDistance = FVector::DistSquared(Context.TransformFragment.Position, OverlappingTransformFragment.Position);
	}

	return true; // Always valid for unlimited range or failed events
}

void UpdateClosestInteractableEntity(const FRecallUpdateClosestInteractableEntityContext& Context)
{
	const FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	const TArray<FMassExtendedEntityHandle> OverlappingEntities = Context.SensorFragment.GetOverlappingEntities(
		Context.InteractorSharedFragment.SensorName);

	struct FRecallInteractableEntityCache
	{
		int32 EntityIndex = INDEX_NONE;
		int32 InteractEventIndex = INDEX_NONE;
		int32 EventPriority = 0;
		float SqrDistance = 0.0f;

		bool Success() const { return InteractEventIndex != INDEX_NONE; }
	};
	TArray<FRecallInteractableEntityCache> InteractableEntities;

	for (int32 OverlappingEntityIndex = 0; OverlappingEntityIndex < OverlappingEntities.Num(); OverlappingEntityIndex++)
	{
		const FMassExtendedEntityHandle& OverlappingEntity = OverlappingEntities[OverlappingEntityIndex];
		if (!EntityManager.IsEntityValid(OverlappingEntity))
		{
			continue;
		}

		const FMassExtendedEntityView OverlappingView(EntityManager, OverlappingEntity);
		const FRecallInteractableFragment* InteractableFragmentPtr = OverlappingView.GetFragmentDataPtr<FRecallInteractableFragment>();
		if (InteractableFragmentPtr == nullptr || !InteractableFragmentPtr->bAllowInteraction)
		{
			continue;
		}

		FText Text;
		const FRecallInteractExecuteContext InteractContext{ Context.ExecutionContext, Context.InteractorEntity };
		const int32 InteractEventIndex = Recall::Interact::Utils::FindInteractEventIndexByInput(
			InteractContext, OverlappingEntity, ERecallInteractInput::Any, false, &Text);
		if (InteractEventIndex == INDEX_NONE && Text.IsEmpty())
		{
			continue;
		}

		// Calculate distance and validate interaction range
		float SqrDistance = 0.0f;
		if (!CalculateDistanceAndValidateRange(Context, *InteractableFragmentPtr,
			OverlappingView, OverlappingEntity, InteractEventIndex, SqrDistance))
		{
			continue; // Skip this entity - beyond interaction range
		}

		FRecallInteractableEntityCache& InteractableEntity = InteractableEntities.AddDefaulted_GetRef();
		InteractableEntity.EntityIndex = OverlappingEntityIndex;
		InteractableEntity.InteractEventIndex = InteractEventIndex;
		InteractableEntity.SqrDistance = SqrDistance;
		
		if (InteractEventIndex != INDEX_NONE)
		{
			const auto* InteractableConstSharedFragmentPtr = OverlappingView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
			const FRecallInteractionEvent& Event = InteractableFragmentPtr->GetInteractEventChecked(InteractEventIndex, InteractableConstSharedFragmentPtr);

			InteractableEntity.EventPriority = Event.EventPriority;
		}
	}

	InteractableEntities.Sort([](const FRecallInteractableEntityCache& A, const FRecallInteractableEntityCache& B)
	{
		if (A.Success() != B.Success())
		{
			return A.Success();
		}
		else if (A.EventPriority != B.EventPriority)
		{
			return A.EventPriority > B.EventPriority;
		}
		else
		{
			return A.SqrDistance < B.SqrDistance;
		}
	});

	if (InteractableEntities.Num())
	{
		const int32 ClosestEntityIndex = InteractableEntities[0].EntityIndex;
		const FMassExtendedEntityHandle& ClosestEntity = OverlappingEntities[ClosestEntityIndex];

		Context.InteractorFragment.ClosestInteractableEntity = ClosestEntity;

		// Only update position index when NOT actively interacting (position locking during interaction)
		if (!Context.InteractorFragment.CurrentInteractEntity.IsValid())
		{
			// Find closest available interaction position (multi-position support)
			const FMassExtendedEntityView ClosestView(EntityManager, ClosestEntity);
			Context.InteractorFragment.ClosestPositionIndex = Recall::Interact::Position::Utils::FindClosestAvailablePosition(
				Context.TransformFragment.Position,
				ClosestView,
				const_cast<FMassExtendedEntityManager&>(EntityManager)
			);
		}
		// else: Keep position index locked during active interaction
	}
	else
	{
		Context.InteractorFragment.ClosestInteractableEntity.Reset();

		// Only reset position index when NOT actively interacting
		if (!Context.InteractorFragment.CurrentInteractEntity.IsValid())
		{
			Context.InteractorFragment.ClosestPositionIndex = INDEX_NONE;
		}
	}
}
} // namespace Recall::Interact::Search::Utils
