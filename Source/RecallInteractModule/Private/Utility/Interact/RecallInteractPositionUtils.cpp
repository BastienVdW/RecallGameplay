// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Interact/RecallInteractPositionUtils.h"

#include "Data/Interact/RecallInteractTypes.h"
#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"

namespace Recall::Interact::Position::Utils
{

FVector GetProjectedInteractionLocation(const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle& InteractorEntity, const FMassExtendedEntityHandle& InteractableEntity,
	bool bUseBoundingBoxProjection)
{
	// Check if interactor is valid for projection calculation
	if (!EntityManager.IsEntityValid(InteractorEntity))
	{
		return FVector::ZeroVector;
	}

	const FMassExtendedEntityView InteractorView(EntityManager, InteractorEntity);
	const FRecallTransformFragment& InteractorTransformFragment = InteractorView.GetFragmentData<FRecallTransformFragment>();

	// Use the position-based variant
	return GetProjectedLocationFromPosition(EntityManager, InteractorTransformFragment.Position, InteractableEntity, bUseBoundingBoxProjection);
}

FVector GetProjectedLocationFromPosition(const FMassExtendedEntityManager& EntityManager,
	const FVector& InteractorPosition, const FMassExtendedEntityHandle& InteractableEntity,
	bool bUseBoundingBoxProjection)
{
	if (!EntityManager.IsEntityValid(InteractableEntity))
	{
		return FVector::ZeroVector;
	}

	const FMassExtendedEntityView InteractableView(EntityManager, InteractableEntity);
	const FRecallTransformFragment& InteractableTransformFragment = InteractableView.GetFragmentData<FRecallTransformFragment>();

	// If not using bounding box projection, return center position
	if (!bUseBoundingBoxProjection)
	{
		return InteractableTransformFragment.Position;
	}

	// Check if interactable has physics body for bounding box
	const FRecallPhysicsBodyFragment* PhysicsBodyFragmentPtr = InteractableView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>();
	if (PhysicsBodyFragmentPtr == nullptr || PhysicsBodyFragmentPtr->Extents.IsNearlyZero())
	{
		return InteractableTransformFragment.Position;
	}

	// Transform interactor position to interactable's local space
	const FTransform InteractableTransform = InteractableTransformFragment.GetTransform();
	
	const FVector LocalPosition = InteractableTransform.InverseTransformPosition(InteractorPosition);
	
	// Project onto axis-aligned bounding box in local space
	const FVector& Extents = PhysicsBodyFragmentPtr->Extents;
	const FVector ClampedLocalPosition = FVector(
		FMath::Clamp(LocalPosition.X, -Extents.X, Extents.X),
		FMath::Clamp(LocalPosition.Y, -Extents.Y, Extents.Y),
		FMath::Clamp(LocalPosition.Z, -Extents.Z, Extents.Z)
	);
	
	// Transform back to world space
	const FVector WorldProjectedPosition = InteractableTransform.TransformPosition(ClampedLocalPosition);
	
	return WorldProjectedPosition;
}

float GetInteractionDistanceSquared(const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle& InteractorEntity, const FMassExtendedEntityHandle& InteractableEntity,
	const FRecallInteractionEvent& Event)
{
	if (!EntityManager.IsEntityValid(InteractorEntity) || !EntityManager.IsEntityValid(InteractableEntity))
	{
		return MAX_FLT;
	}

	const FMassExtendedEntityView InteractorView(EntityManager, InteractorEntity);
	const FRecallTransformFragment* InteractorTransformFragmentPtr = InteractorView.GetFragmentDataPtr<FRecallTransformFragment>();
	if (InteractorTransformFragmentPtr == nullptr)
	{
		return MAX_FLT;
	}

	const FVector InteractorPosition = InteractorTransformFragmentPtr->Position;
	const FVector InteractablePosition = GetProjectedInteractionLocation(EntityManager, InteractorEntity, InteractableEntity, Event.bUseBoundingBoxProjection);
	
	return FVector::DistSquared(InteractorPosition, InteractablePosition);
}

int32 FindClosestAvailablePosition(
	const FVector& InteractorLocation,
	const FMassExtendedEntityView& InteractableView,
	const FMassExtendedEntityManager& EntityManager)
{
	const FRecallInteractableFragment& InteractableFragment =
		InteractableView.GetFragmentData<FRecallInteractableFragment>();

	// Backward compatibility: empty array means no multi-position support
	if (InteractableFragment.InteractionPositions.IsEmpty())
	{
		return INDEX_NONE;
	}

	// Get entity transform to convert relative positions to world space
	const FRecallTransformFragment& TransformFragment =
		InteractableView.GetFragmentData<FRecallTransformFragment>();
	const FTransform EntityTransform(TransformFragment.Rotation, TransformFragment.Position);

	// Build distance-sorted array with position indices
	TArray<TPair<float, int32>> DistanceIndexPairs;
	DistanceIndexPairs.Reserve(InteractableFragment.InteractionPositions.Num());

	for (int32 i = 0; i < InteractableFragment.InteractionPositions.Num(); ++i)
	{
		const FVector WorldLocation = InteractableFragment.InteractionPositions[i].GetWorldLocation(EntityTransform);
		const float SqrDist = FVector::DistSquared(InteractorLocation, WorldLocation);
		DistanceIndexPairs.Emplace(SqrDist, i);
	}

	// Stable sort for determinism (tie-breaking by index)
	DistanceIndexPairs.StableSort([](const TPair<float, int32>& A, const TPair<float, int32>& B)
	{
		return A.Key < B.Key || (A.Key == B.Key && A.Value < B.Value);
	});

	// Return first unoccupied position
	for (const TPair<float, int32>& Pair : DistanceIndexPairs)
	{
		if (!IsPositionOccupied(InteractableView, Pair.Value, EntityManager))
		{
			return Pair.Value;
		}
	}

	return INDEX_NONE;  // All positions occupied
}

bool IsPositionOccupied(
	const FMassExtendedEntityView& InteractableView,
	int32 PositionIndex,
	const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle* IgnoredInstigator)
{
	// INDEX_NONE is never occupied (fallback to center position)
	if (PositionIndex == INDEX_NONE)
	{
		return false;
	}

	const FRecallInteractableFragment& InteractableFragment =
		InteractableView.GetFragmentData<FRecallInteractableFragment>();

	// Check all in-progress interactors
	for (const FMassExtendedEntityHandle& InstigatorEntity : InteractableFragment.InProgressInstigators)
	{
		if (!EntityManager.IsEntityValid(InstigatorEntity))
			continue;

		if (IgnoredInstigator && InstigatorEntity == *IgnoredInstigator)
			continue; // Ignore self occupying the position

		const FMassExtendedEntityView InstigatorView(EntityManager, InstigatorEntity);
		if (const FRecallInteractorFragment* InstigatorFragmentPtr =
			InstigatorView.GetFragmentDataPtr<FRecallInteractorFragment>())
		{
			if (InstigatorFragmentPtr->ClosestPositionIndex == PositionIndex)
			{
				return true;  // Position occupied by someone else
			}
		}
	}

	return false;  // Position available
}

bool IsPositionOccupied(
	const FMassExtendedEntityView& InteractableView,
	int32 PositionIndex,
	const FMassExtendedEntityManager& EntityManager)
{
	return IsPositionOccupied(InteractableView, PositionIndex, EntityManager, nullptr);
}

bool CalculateInteractionLocationAndDistance(
	const FVector& InteractorLocation,
	const FMassExtendedEntityHandle& InteractableEntity,
	const FMassExtendedEntityView& InteractableView,
	const FMassExtendedEntityManager& EntityManager,
	int32& OutPositionIndex,
	FVector& OutInteractLocation,
	float& OutDistanceSquared)
{
	const FRecallInteractableFragment* InteractableFragmentPtr = InteractableView.GetFragmentDataPtr<FRecallInteractableFragment>();

	if (!InteractableFragmentPtr)
	{
		return false;
	}

	// Check if this is a multi-position interactable
	if (InteractableFragmentPtr->HasMultiplePositions())
	{
		// Find the closest available position
		OutPositionIndex = FindClosestAvailablePosition(
			InteractorLocation, InteractableView, EntityManager);

		// Return false if no positions available
		if (OutPositionIndex == INDEX_NONE)
		{
			return false;
		}

		// Get the world location of this position
		const FRecallTransformFragment* InteractableTransformPtr = InteractableView.GetFragmentDataPtr<FRecallTransformFragment>();
		if (!InteractableTransformPtr)
		{
			return false;
		}

		const FTransform& InteractableTransform = InteractableTransformPtr->GetTransform();
		OutInteractLocation = InteractableFragmentPtr->InteractionPositions[OutPositionIndex].GetWorldLocation(InteractableTransform);
		OutDistanceSquared = FVector::DistSquared(InteractorLocation, OutInteractLocation);
		return true;
	}
	else
	{
		// Use projected location for single-position interactables
		OutPositionIndex = INDEX_NONE;
		OutInteractLocation = GetProjectedLocationFromPosition(
			EntityManager, InteractorLocation, InteractableEntity, true);
		OutDistanceSquared = FVector::DistSquared(InteractorLocation, OutInteractLocation);
		return true;
	}
}

} // namespace Recall::Interact::Position::Utils
