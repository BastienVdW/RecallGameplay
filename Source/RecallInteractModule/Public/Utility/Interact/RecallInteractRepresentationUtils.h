// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassExtendedEntityHandle;
struct FMassExtendedEntityManager;
struct FMassExtendedExecutionContext;
struct FRecallInteractionRepresentation;
struct FRecallInteractorFragment;
struct FRecallInteractState;
class URecallActorSubsystem;

/**
 * Context for interaction representation operations.
 * Contains execution context and actor subsystem for full representation functionality.
 */
struct FRecallInteractRepresentationContext
{
	FMassExtendedExecutionContext& ExecutionContext;
	const URecallActorSubsystem& ActorSubsystem;
};

namespace Recall::Interact::Representation::Utils
{

/**
 * Check if an interaction should show progress UI based on its duration.
 * @param Context Representation context containing execution context and actor subsystem
 * @param InteractorFragment Interactor fragment containing current interaction state
 * @param InteractableEntity The interactable entity to check
 * @return True if progress should be shown, false otherwise
 */
RECALLINTERACTMODULE_API extern bool ShouldShowProgress(const FRecallInteractRepresentationContext& Context,
	const FRecallInteractorFragment& InteractorFragment, const FMassExtendedEntityHandle& InteractableEntity);

/**
 * Get the world location for displaying interaction UI for an interactable entity.
 * Supports multi-position interactions by accepting an optional position index.
 * Prioritizes position location (if index valid), then socket location, then falls back to center position.
 * @param Context Representation context containing execution context and actor subsystem
 * @param InteractableEntity The interactable entity
 * @param InteractorEntity The interactor entity for projection calculations
 * @param PositionIndex Optional position index (INDEX_NONE = use socket/center fallback)
 * @return World location for interaction UI display
 */
RECALLINTERACTMODULE_API extern FVector GetInteractableLocation(const FRecallInteractRepresentationContext& Context,
	const FMassExtendedEntityHandle& InteractableEntity, const FMassExtendedEntityHandle& InteractorEntity,
	int32 PositionIndex = INDEX_NONE);

/**
 * Build interaction representation data for UI display.
 * @param Context Representation context containing execution context and actor subsystem
 * @param InteractorEntity The interactor entity
 * @param InteractableEntity The interactable entity to build representation for
 * @param bIsContextual Whether this is a contextual interaction
 * @return Interaction representation data for UI
 */
	RECALLINTERACTMODULE_API extern FRecallInteractionRepresentation GetInteractionRepresentation(
		const FRecallInteractRepresentationContext& Context,
		const FMassExtendedEntityHandle& InteractorEntity, const FMassExtendedEntityHandle& InteractableEntity, bool bIsContextual);

/**
 * Get complete interaction state for an interactor.
 * @param Context Representation context containing execution context and actor subsystem
 * @param InteractorEntity The interactor entity
 * @param InteractorFragment Interactor fragment containing interaction state
 * @return Complete interaction state for UI representation
 */
RECALLINTERACTMODULE_API extern FRecallInteractState GetInteractInfo(const FRecallInteractRepresentationContext& Context,
	const FMassExtendedEntityHandle& InteractorEntity, const FRecallInteractorFragment& InteractorFragment);
	
}