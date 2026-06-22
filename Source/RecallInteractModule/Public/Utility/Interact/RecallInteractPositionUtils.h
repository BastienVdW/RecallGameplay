// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassExtendedEntityManager;
struct FMassExtendedEntityHandle;
struct FMassExtendedEntityView;
struct FRecallInteractionEvent;

namespace Recall::Interact::Position::Utils
{

/// Calculate closest point on interactable's bounding box to interactor position.
/// @param EntityManager The entity manager
/// @param InteractorEntity The entity doing the interaction
/// @param InteractableEntity The entity being interacted with
/// @param bUseBoundingBoxProjection If true, project onto bounding box; if false, use center position
/// @return World space location for interaction display/calculation
RECALLINTERACTMODULE_API extern FVector GetProjectedInteractionLocation(
	const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle& InteractorEntity,
	const FMassExtendedEntityHandle& InteractableEntity,
	bool bUseBoundingBoxProjection = true);

/// Calculate closest point on interactable's bounding box to a specific world position.
/// @param EntityManager The entity manager
/// @param InteractorPosition The world position to project from
/// @param InteractableEntity The entity being interacted with
/// @param bUseBoundingBoxProjection If true, project onto bounding box; if false, use center position
/// @return World space location for interaction display/calculation
RECALLINTERACTMODULE_API extern FVector GetProjectedLocationFromPosition(
	const FMassExtendedEntityManager& EntityManager,
	const FVector& InteractorPosition,
	const FMassExtendedEntityHandle& InteractableEntity,
	bool bUseBoundingBoxProjection = true);

/// Calculate squared distance between interactor and interactable using bounding box projection if enabled.
/// @param EntityManager The entity manager
/// @param InteractorEntity The entity doing the interaction
/// @param InteractableEntity The entity being interacted with
/// @param Event The interaction event containing bounding box projection setting
/// @return Squared distance for comparison
RECALLINTERACTMODULE_API extern float GetInteractionDistanceSquared(
	const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle& InteractorEntity,
	const FMassExtendedEntityHandle& InteractableEntity,
	const FRecallInteractionEvent& Event);

/// Find the closest available (unoccupied) interaction position.
/// @param InteractorLocation World position of the interactor entity
/// @param InteractableView Entity view of the target interactable
/// @param EntityManager Manager for querying entity validity and fragments
/// @return Position index (0-N) or INDEX_NONE if no positions available
RECALLINTERACTMODULE_API extern int32 FindClosestAvailablePosition(
	const FVector& InteractorLocation,
	const FMassExtendedEntityView& InteractableView,
	const FMassExtendedEntityManager& EntityManager);

/// Check if a specific interaction position is occupied.
/// @param InteractableView Entity view of the interactable
/// @param PositionIndex Index of position to check
/// @param EntityManager Manager for querying entity fragments
/// @return true if position is occupied, false if available
RECALLINTERACTMODULE_API extern bool IsPositionOccupied(
	const FMassExtendedEntityView& InteractableView,
	int32 PositionIndex,
	const FMassExtendedEntityManager& EntityManager);

/// Overload that allows ignoring a specific instigator (e.g., the current interactor),
/// so a position occupied by self does not count as occupied.
RECALLINTERACTMODULE_API extern bool IsPositionOccupied(
	const FMassExtendedEntityView& InteractableView,
	int32 PositionIndex,
	const FMassExtendedEntityManager& EntityManager,
	const FMassExtendedEntityHandle* IgnoredInstigator);

/// Calculate interaction location and distance for an interactable entity.
/// Handles both multi-position and single-position interactables.
/// @param InteractorLocation The world position of the interactor
/// @param InteractableEntity The entity being interacted with
/// @param InteractableView The entity view of the interactable
/// @param EntityManager The entity manager
/// @param OutPositionIndex Output position index (INDEX_NONE for single-position or center)
/// @param OutInteractLocation Output interaction location in world space
/// @param OutDistanceSquared Output squared distance to interaction location
/// @return true if interaction location was calculated successfully, false if no positions available
RECALLINTERACTMODULE_API extern bool CalculateInteractionLocationAndDistance(
	const FVector& InteractorLocation,
	const FMassExtendedEntityHandle& InteractableEntity,
	const FMassExtendedEntityView& InteractableView,
	const FMassExtendedEntityManager& EntityManager,
	int32& OutPositionIndex,
	FVector& OutInteractLocation,
	float& OutDistanceSquared);

} // namespace Recall::Interact::Position::Utils