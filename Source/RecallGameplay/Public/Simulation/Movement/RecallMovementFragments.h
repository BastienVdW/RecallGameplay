// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "RecallMovementTypes.h"

#include "RecallMovementFragments.generated.h"

USTRUCT() struct RECALLGAMEPLAY_API FRecallCrowdMovementTag : public FMassExtendedTag { GENERATED_BODY() };

/**
 * Tag to identify entities which are in the process of traversing a nav link.
 */
USTRUCT() struct RECALLGAMEPLAY_API FRecallNavLinkTraversalTag : public FMassExtendedTag { GENERATED_BODY() };

/**
 * Fragment containing movement input data for an entity.
 *
 * This fragment stores the current movement input state, including the desired movement direction
 * and any pending jump requests. Movement direction is expressed in world coordinates.
 */
USTRUCT()
struct RECALLGAMEPLAY_API FRecallMovementFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	/** Movement direction in world coordinates. Represents the desired movement input direction. */
	UPROPERTY(VisibleAnywhere)
	FVector2f MovementDirection = FVector2f::ZeroVector;

	/** Flag indicating a jump request. Set to true when the entity wants to jump. */
	UPROPERTY(VisibleAnywhere)
	uint8 bWantsToJump = false;

	/** Clears the current movement input by resetting direction to zero. */
	FORCEINLINE void Stop()
	{
		MovementDirection = FVector2f::ZeroVector;
	}
};

/**
 * Const shared fragment containing movement configuration shared across multiple entities.
 *
 * This fragment stores movement settings that are typically shared by groups of entities
 * (e.g., all entities of the same character type). As a const shared fragment, these settings
 * cannot be modified during gameplay and provide memory-efficient configuration storage.
 */
USTRUCT()
struct RECALLGAMEPLAY_API FRecallMovementSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	/** Movement configuration settings shared across entities (e.g., max speed, acceleration). */
	UPROPERTY(VisibleAnywhere)
	FRecallMovementSettings MovementSettings;
};
