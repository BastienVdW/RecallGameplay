// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Desync/RecallDesyncLog.h"

struct FMassExtendedEntityHandle;
class URecallNavigationSubsystem;

struct FRecallNavigationContext
{
	struct FMassExtendedExecutionContext& ExecutionContext;
	const FMassExtendedEntityHandle& Entity;
	const struct FRecallNavigationAgentConstSharedFragment& AgentConstSharedFragment;
	const struct FRecallTransformFragment& TransformFragment;
	struct FRecallNavigationAgentFragment& NavigationFragment;
	struct FRecallMovementFragment& MovementFragment;
	struct FRecallNavLinkTraversalFragment& NavLinkTraversalFragment;
	
	FCriticalSection& DoneDataGuard;
	TArray<FMassExtendedEntityHandle>& EntitiesToSignalDone;
	
	FCriticalSection& AbortDataGuard;
	TArray<FMassExtendedEntityHandle>& EntitiesToSignalAbort;
	
	FCriticalSection& StartNavLinkDataGuard;
	TArray<FMassExtendedEntityHandle>& StartNavLinkEntities;
};

namespace Recall::Navigation::Utils
{

/**
 * Convert NavMesh path points to generic path points and populate the path follower.
 * This function handles the conversion of FRecallNavigationPathPoint to FRecallPathPoint,
 * preserving NavLink metadata and starting the path following.
 *
 * @param NavMeshPathPoints The path points returned from the NavMesh system
 * @param PathFollowerFragment The path follower fragment to populate with converted points
 * @return true if conversion succeeded and path was started, false otherwise
 */
RECALLNAVIGATION_API extern bool StartPathFollowingFromNavMeshPoints(
	const TArray<struct FRecallNavigationPathPoint>& NavMeshPathPoints,
	struct FRecallPathFollowerFragment& PathFollowerFragment);

/**
 * Refresh the navigation path.
 */
RECALLNAVIGATION_API extern void UpdateNavigationPath(
	const FRecallNavigationContext& NavigationContext, URecallNavigationSubsystem& NavigationSystem);

/**
 * Follow the navigation path.
 */
RECALLNAVIGATION_API extern void FollowNavigationPath(const FRecallNavigationContext& NavigationContext);

RECALLNAVIGATION_API extern void RegisterGeneratedLinksProxy(UWorld* World);
	
#if RECALL_DESYNC_LOG
RECALLNAVIGATION_API extern void DesyncLogNavigationPath(const FRecallNavigationContext& NavigationContext);
#endif // RECALL_DESYNC_LOG
	
} // namespace Recall::Navigation::Utils
