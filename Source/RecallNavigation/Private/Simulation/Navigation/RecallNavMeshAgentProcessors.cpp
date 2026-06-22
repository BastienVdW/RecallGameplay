// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavMeshAgentProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Navigation/RecallNavigationSignalTypes.h"
#include "Simulation/Navigation/RecallNavigationFragments.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Navigation/RecallNavigationSubsystem.h"
#include "Utility/Navigation/RecallNavigationUtils.h"

//----------------------------------------------------------------------//
// URecallNavMeshAgentConstructor
//----------------------------------------------------------------------//
URecallNavMeshAgentConstructor::URecallNavMeshAgentConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallNavMeshPathRequestFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallNavMeshAgentConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavMeshAgentConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallNavMeshPathRequestFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallNavigationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavMeshAgentConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavMeshAgent_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallNavigationSubsystem& NavigationSystem = Context.GetMutableSubsystemChecked<URecallNavigationSubsystem>();

		const TArrayView<FRecallNavMeshPathRequestFragment> NavMeshPathRequestList = Context.GetMutableFragmentView<FRecallNavMeshPathRequestFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallNavMeshPathRequestFragment& NavMeshPathRequestFragment = NavMeshPathRequestList[EntityIndex];
		}
	});
}

//----------------------------------------------------------------------//
// URecallNavMeshAgentDestructor
//----------------------------------------------------------------------//
URecallNavMeshAgentDestructor::URecallNavMeshAgentDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallNavMeshPathRequestFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallNavMeshAgentDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavMeshAgentDestructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallNavMeshPathRequestFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallNavigationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);

}

void URecallNavMeshAgentDestructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavMeshAgent_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallNavigationSubsystem& NavigationSystem = Context.GetMutableSubsystemChecked<URecallNavigationSubsystem>();

		const TArrayView<FRecallNavMeshPathRequestFragment> NavMeshPathRequestList = Context.GetMutableFragmentView<FRecallNavMeshPathRequestFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallNavMeshPathRequestFragment& NavMeshPathRequestFragment = NavMeshPathRequestList[EntityIndex];
			NavigationSystem.ReleaseAsyncPath(NavMeshPathRequestFragment.PathHandle);
		}
	});
}

//----------------------------------------------------------------------//
// URecallNavMeshAgentProcessor
//----------------------------------------------------------------------//
URecallNavMeshAgentProcessor::URecallNavMeshAgentProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallNavMeshAgentProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavMeshAgentProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallNavMeshPathRequestFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPathFollowerFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallNavigationAgentConstSharedFragment>();
	EntityQuery.AddConstSharedRequirement<FRecallPathFollowingConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallNavigationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);

	ProcessorRequirements.AddSubsystemRequirement<URecallNavigationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavMeshAgentProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavMeshAgent_Execute);

	URecallNavigationSubsystem& NavigationSystem = Context.GetMutableSubsystemChecked<URecallNavigationSubsystem>();

	EntityQuery.ForEachEntityChunk(Context,
		[&NavigationSystem](FMassExtendedExecutionContext& Context)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallNavigationAgentConstSharedFragment& AgentConstSharedFragment = Context.GetConstSharedFragment<FRecallNavigationAgentConstSharedFragment>();
		const FRecallNavigationAgentSettings& AgentSettings = AgentConstSharedFragment.Agent;
		const FRecallPathFollowingConstSharedFragment& PathFollowingConstSharedFragment = Context.GetConstSharedFragment<FRecallPathFollowingConstSharedFragment>();
		const FRecallPathFollowingSettings& PathFollowingSettings = PathFollowingConstSharedFragment.Settings;

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TArrayView<FRecallNavMeshPathRequestFragment> NavMeshPathRequestList = Context.GetMutableFragmentView<FRecallNavMeshPathRequestFragment>();
		const TArrayView<FRecallPathFollowerFragment> PathFollowerList = Context.GetMutableFragmentView<FRecallPathFollowerFragment>();

		// Handle path requests from PathFollower Fragment (generic pattern allows other navigation systems)
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallNavMeshPathRequestFragment& NavMeshPathRequest = NavMeshPathRequestList[EntityIndex];
			FRecallPathFollowerFragment& PathFollower = PathFollowerList[EntityIndex];
			const FRecallTransformFragment& Transform = TransformList[EntityIndex];

			// Check if path follower is requesting a new path from NavMesh
			if (PathFollower.IsPathRequested())
			{
				const FVector& TargetLocation = PathFollower.GetDestination();
				const float SqrDestinationThreshold = PathFollowingSettings.DestinationThreshold * PathFollowingSettings.DestinationThreshold;
				const float SqrDistance = FVector::DistSquared(Transform.Position, TargetLocation);

				// Check if already at destination
				if (SqrDistance < SqrDestinationThreshold)
				{
					PathFollower.Stop();
					SignalSystem.SignalEntity(Recall::Navigation::Signals::MoveAtAbort, Entity);
				}
				else
				{
					// Request a new NavMesh path
					NavMeshPathRequest.TargetLocation = TargetLocation;

					if (AgentSettings.bPathAsync)
					{
						NavigationSystem.RequestPathAsync(NavMeshPathRequest.PathHandle,
							Transform.Position, TargetLocation,
							PathFollowingSettings.Radius, PathFollowingSettings.AgentHeight);
						PathFollower.ClearPathRequest();
					}
					else
					{
						// Sync path request - generate path immediately
						TArray<FRecallNavigationPathPoint> NavMeshPathPoints;
						if (NavigationSystem.RequestPathSync(Transform.Position, TargetLocation, NavMeshPathPoints))
						{
							Recall::Navigation::Utils::StartPathFollowingFromNavMeshPoints(NavMeshPathPoints, PathFollower);
							PathFollower.ClearPathRequest();
						}
						else
						{
							// Path generation failed - clear path to signal failure
							PathFollower.Stop();
							SignalSystem.SignalEntity(Recall::Navigation::Signals::MoveAtAbort, Entity);
						}
					}
				}
			}
			else if (NavMeshPathRequest.IsWaitingPath())
			{
				// Check if async path is ready
				if (AgentSettings.bPathAsync && NavigationSystem.IsRequestFinished(NavMeshPathRequest.PathHandle))
				{
					TArray<FRecallNavigationPathPoint> NavMeshPathPoints;
					if (NavigationSystem.GetAsyncPathResult(NavMeshPathRequest.PathHandle, NavMeshPathPoints))
					{
						Recall::Navigation::Utils::StartPathFollowingFromNavMeshPoints(NavMeshPathPoints, PathFollower);
						PathFollower.ClearPathRequest();
					}
					else
					{
						// Path generation failed - clear path to signal failure
						PathFollower.Stop();
						SignalSystem.SignalEntity(Recall::Navigation::Signals::MoveAtAbort, Entity);
					}

					NavigationSystem.ReleaseAsyncPath(NavMeshPathRequest.PathHandle);
				}
			}
		}
	});

	NavigationSystem.TickPathQueue();
}
