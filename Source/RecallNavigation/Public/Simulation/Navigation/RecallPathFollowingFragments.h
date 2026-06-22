// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "Simulation/Navigation/RecallPathFollowingSettings.h"
#include "Simulation/Crowd/RecallCrowdAgentCollisionSettings.h"

#include "RecallPathFollowingFragments.generated.h"

/**
 * Path point flags for metadata about specific waypoints
 */
namespace Recall::PathFollowing
{
	constexpr uint8 PathPointFlag_NavLink = 1 << 0; // Waypoint involves NavLink traversal
}

/**
 * Generic path point type used by all path following systems (NavMesh, Grid, custom algorithms)
 * This allows different path generation systems to feed paths into a shared path following processor
 */
USTRUCT()
struct RECALLNAVIGATION_API FRecallPathPoint
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	uint8 Flags = 0; // See Recall::PathFollowing::PathPointFlag_* for values (NavLink, etc.)

	
	UPROPERTY()
	FNavLinkId CustomNavLinkId;
	
	/** Get NavLink flag from this path point */
	FORCEINLINE bool IsNavLink() const
	{
		return (Flags & Recall::PathFollowing::PathPointFlag_NavLink) != 0;
	}
};

/**
 * Generic path follower fragment
 * Stores the current path and follows it towards the destination
 * Used by all path following processors regardless of path source (NavMesh, Grid, etc.)
 */
UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallPathFollowerFlag : uint16
{
	None			= 0,
	Moving			= 1 << 0,	// Currently following a path
	Arrived			= 1 << 1,	// Reached the destination
	Stopped			= 1 << 2,	// Stopped (either by command or failed path)
	PathRequested	= 1 << 3,	// Path generation has been requested (waiting for path generation system)
};
ENUM_CLASS_FLAGS(ERecallPathFollowerFlag)

inline uint16& operator|=(uint16& Lhs, ERecallPathFollowerFlag Rhs) { return Lhs |= static_cast<uint16>(Rhs); }
constexpr void EnumAddFlags(uint16& Flags, ERecallPathFollowerFlag FlagsToAdd) { Flags |= static_cast<uint16>(FlagsToAdd); }
constexpr void EnumRemoveFlags(uint16& Flags, ERecallPathFollowerFlag FlagsToRemove) { Flags = Flags & ~static_cast<uint16>(FlagsToRemove); }
constexpr bool EnumHasAnyFlags(uint16 Flags, ERecallPathFollowerFlag Contains) { return (Flags & static_cast<uint16>(Contains)) != 0; }

USTRUCT()
struct RECALLNAVIGATION_API FRecallPathFollowerFragment : public FMassFragment
{
	GENERATED_BODY()

public:
	/** Start following the path */
	FORCEINLINE void Start()
	{
		checkf(HasPath(), TEXT("%hs Must have a valid path to start"), __FUNCTION__);

		CurrentSegmentIndex = 0;
		EnumAddFlags(FollowerFlags, ERecallPathFollowerFlag::Moving);
		EnumRemoveFlags(FollowerFlags, ERecallPathFollowerFlag::Arrived | ERecallPathFollowerFlag::Stopped);
	}

	/** Stop following the path */
	FORCEINLINE void Stop()
	{
		CurrentSegmentIndex = INDEX_NONE;
		ClearPath();
		ClearPathRequest();
		EnumRemoveFlags(FollowerFlags, ERecallPathFollowerFlag::Moving);
		EnumAddFlags(FollowerFlags, ERecallPathFollowerFlag::Stopped);
	}

	/** Check if actively following a path */
	FORCEINLINE bool IsMoving() const
	{
		return EnumHasAnyFlags(FollowerFlags, ERecallPathFollowerFlag::Moving);
	}

	/** Check if destination has been reached */
	FORCEINLINE bool HasArrived() const
	{
		return EnumHasAnyFlags(FollowerFlags, ERecallPathFollowerFlag::Arrived);
	}

	/** Check if path following has stopped */
	FORCEINLINE bool HasStopped() const
	{
		return EnumHasAnyFlags(FollowerFlags, ERecallPathFollowerFlag::Stopped);
	}

	/** Check if we have a valid path to follow */
	FORCEINLINE bool HasPath() const
	{
		return PathPoints.Num() > 0;
	}

	/** Move to the next waypoint */
	FORCEINLINE void AdvanceSegment()
	{
		CurrentSegmentIndex++;
	}

	/** Get current waypoint index */
	FORCEINLINE int32 GetCurrentSegmentIndex() const
	{
		return CurrentSegmentIndex;
	}

	/** Get total number of waypoints */
	FORCEINLINE int32 GetPathLength() const
	{
		return PathPoints.Num();
	}

	/** Get current waypoint location (or zero if invalid) */
	FORCEINLINE FVector GetCurrentWaypoint() const
	{
		if (PathPoints.IsValidIndex(CurrentSegmentIndex))
		{
			return PathPoints[CurrentSegmentIndex].Location;
		}
		return FVector::ZeroVector;
	}

	/** Get destination location */
	FORCEINLINE const FVector& GetDestination() const
	{
		return TargetDestination;
	}

	/** Set the destination this path is targeting */
	FORCEINLINE void SetTargetDestination(const FVector& Destination)
	{
		TargetDestination = Destination;
	}

	/** Clear the current path */
	FORCEINLINE void ClearPath()
	{
		PathPoints.Reset();
		CurrentSegmentIndex = INDEX_NONE;
	}

	/** Validate that path is not completely out of bounds */
	FORCEINLINE bool IsValidIndex() const
	{
		return CurrentSegmentIndex != INDEX_NONE && CurrentSegmentIndex < PathPoints.Num();
	}

	/** Request a new path to be generated (sets dirty flag for path generation systems) */
	FORCEINLINE void RequestPath(const FVector& Destination)
	{
		TargetDestination = Destination;
		EnumAddFlags(FollowerFlags, ERecallPathFollowerFlag::PathRequested);
	}

	/** Check if a path request is pending (for path generation systems to poll) */
	FORCEINLINE bool IsPathRequested() const
	{
		return EnumHasAnyFlags(FollowerFlags, ERecallPathFollowerFlag::PathRequested);
	}

	/** Clear the path request flag after generating the path (called by path generation system) */
	FORCEINLINE void ClearPathRequest()
	{
		EnumRemoveFlags(FollowerFlags, ERecallPathFollowerFlag::PathRequested);
	}

	/** Mark that the current segment has been processed (for one-time segment events like NavLinks) */
	FORCEINLINE void SetMoveSegmentDirty()
	{
		bMoveSegmentDirty = true;
	}

	/** Check if segment transition needs processing */
	FORCEINLINE bool IsMoveSegmentDirty() const
	{
		return bMoveSegmentDirty;
	}

	/** Clear the segment dirty flag (call after processing segment event) */
	FORCEINLINE void ClearMoveSegmentDirtyFlag()
	{
		bMoveSegmentDirty = false;
	}

public:
	/** Generic path points from any path generation system */
	UPROPERTY(VisibleAnywhere)
	TArray<FRecallPathPoint> PathPoints;

	/** Current waypoint index in the path */
	UPROPERTY(VisibleAnywhere)
	int32 CurrentSegmentIndex = INDEX_NONE;

	/** State flags for path following */
	UPROPERTY(VisibleAnywhere, meta=(Bitmask, BitmaskEnum="/Script/RecallNavigation.ERecallPathFollowerFlag"))
	uint16 FollowerFlags = 0;

	/** Target destination this path is heading towards */
	UPROPERTY(VisibleAnywhere)
	FVector TargetDestination = FVector::ZeroVector;

	/** Segment dirty flag - set when advancing to a new segment for one-time segment event processing (NavLinks, etc.) */
	UPROPERTY(VisibleAnywhere)
	uint8 bMoveSegmentDirty : 1 = 0;
};

template <>
struct TMassFragmentTraits<FRecallPathFollowerFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };

/**
 * Shared constant fragment for path following configuration
 * All navigation agents (NavMesh and Grid) must have this fragment for the path follower processor to work
 */
USTRUCT()
struct RECALLNAVIGATION_API FRecallPathFollowingConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FRecallPathFollowingSettings Settings;
};

/**
 * Shared constant fragment for crowd agent collision configuration
 * All navigation agents that use crowd avoidance must have this fragment
 * Contains agent-specific settings for crowd collision (height, etc.)
 */
USTRUCT()
struct RECALLNAVIGATION_API FRecallCrowdAgentCollisionConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallCrowdAgentCollisionSettings Settings;
};
