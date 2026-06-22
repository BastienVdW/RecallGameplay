// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPathFollowerProcessors.h"

#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Navigation/RecallNavigationSignalTypes.h"
#include "Simulation/Navigation/RecallNavigationFragments.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Navigation/RecallNavLinkFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Navigation/RecallNavigationSubsystem.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#include "DrawDebugHelpers.h"
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

//----------------------------------------------------------------------//
// URecallPathFollowerProcessor
//----------------------------------------------------------------------//
URecallPathFollowerProcessor::URecallPathFollowerProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallPathFollowerProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallPathFollowerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet InvalidTags;
	InvalidTags.Add(FRecallNavLinkTraversalTag::StaticStruct());

	EntityQuery.AddRequirement<FRecallPathFollowerFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallMovementFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallNavLinkTraversalFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirements<EMassFragmentPresence::None>(InvalidTags);
	EntityQuery.AddConstSharedRequirement<FRecallPathFollowingConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallNavigationSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallPathFollowerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_PathFollower_Execute);

	EntityQuery.ParallelForEachEntityChunk(Context,
		[](FMassExecutionContext& Context)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallPathFollowingConstSharedFragment& PathFollowingFragment = Context.GetConstSharedFragment<FRecallPathFollowingConstSharedFragment>();

		const FRecallPathFollowingSettings& PathFollowingSettings = PathFollowingFragment.Settings;
		const float SqrDestinationThreshold = PathFollowingSettings.DestinationThreshold * PathFollowingSettings.DestinationThreshold;

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		const TArrayView<FRecallPathFollowerFragment> PathFollowerList = Context.GetMutableFragmentView<FRecallPathFollowerFragment>();
		const TArrayView<FRecallMovementFragment> MovementList = Context.GetMutableFragmentView<FRecallMovementFragment>();
		const TArrayView<FRecallNavLinkTraversalFragment> NavLinkTraversalList = Context.GetMutableFragmentView<FRecallNavLinkTraversalFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			
			FRecallPathFollowerFragment& PathFollowerFragment = PathFollowerList[EntityIndex];
			FRecallMovementFragment& MovementFragment = MovementList[EntityIndex];
			FRecallNavLinkTraversalFragment& NavLinkTraversalFragment = NavLinkTraversalList[EntityIndex];
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];

			if (!PathFollowerFragment.IsMoving())
			{
				continue;
			}

			// Follow the path
			while (PathFollowerFragment.GetCurrentSegmentIndex() + 1 < PathFollowerFragment.GetPathLength())
			{
				const FRecallPathPoint& CurrentPoint = PathFollowerFragment.PathPoints[PathFollowerFragment.GetCurrentSegmentIndex()];
				const FRecallPathPoint& NextPoint = PathFollowerFragment.PathPoints[PathFollowerFragment.GetCurrentSegmentIndex() + 1];

				// Check if segment transition needs processing (for one-time events like NavLinks)
				if (PathFollowerFragment.IsMoveSegmentDirty())
				{
					PathFollowerFragment.ClearMoveSegmentDirtyFlag();

					// Handle NavLink traversal if this waypoint involves a NavLink
					if (CurrentPoint.IsNavLink() && ensureAlwaysMsgf(CurrentPoint.CustomNavLinkId != FNavLinkId::Invalid,
							TEXT("%hs LinkProxyClass should be set on RecastNavMesh"), __FUNCTION__))
					{
						NavLinkTraversalFragment.CustomNavLinkId = CurrentPoint.CustomNavLinkId.GetId();
						NavLinkTraversalFragment.StartLocation = CurrentPoint.Location;
						NavLinkTraversalFragment.EndLocation = NextPoint.Location;
									
						Context.Defer().AddTag<FRecallNavLinkTraversalTag>(Entity);
						break;
					}
				}

				const FVector DestinationVector = NextPoint.Location - TransformFragment.Position;

				// Reached destination
				if (DestinationVector.SizeSquared2D() < SqrDestinationThreshold)
				{
					PathFollowerFragment.AdvanceSegment();
					PathFollowerFragment.SetMoveSegmentDirty();
				}
				else
				{
					// Move toward destination
					const FVector2D Movement2D = static_cast<FVector2D>(DestinationVector.GetSafeNormal2D());
					MovementFragment.MovementDirection = static_cast<FVector2f>(Movement2D);
					break;
				}
			}

			// Reached final destination
			if (PathFollowerFragment.GetCurrentSegmentIndex() + 1 >= PathFollowerFragment.GetPathLength())
			{
				MovementFragment.Stop();
				PathFollowerFragment.Stop();

				SignalSystem.SignalEntity(Recall::Navigation::Signals::MoveAtDone, Entity);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallPathFollowerRepresentationProcessor
//----------------------------------------------------------------------//
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
static bool bDebugShowNavigationPath = false;
static FAutoConsoleVariableRef CVarRecallShowNavigationPath(
	TEXT("recall.stateTree.ShowNavigationPath"),
	bDebugShowNavigationPath,
	TEXT("Show Navigation Path")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

URecallPathFollowerRepresentationProcessor::URecallPathFollowerRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallPathFollowerRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallPathFollowerRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	EntityQuery.AddRequirement<FRecallPathFollowerFragment>(EMassFragmentAccess::ReadOnly);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallPathFollowerRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	QUICK_SCOPE_CYCLE_COUNTER(Recall_PathFollower_Representation);

	if (!bDebugShowNavigationPath)
	{
		return;
	}

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const TConstArrayView<FRecallPathFollowerFragment> PathFollowerList = Context.GetFragmentView<FRecallPathFollowerFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallPathFollowerFragment& PathFollower = PathFollowerList[EntityIndex];
			if (!PathFollower.HasPath())
			{
				continue;
			}

			for (int32 PathPointIndex = 0; PathPointIndex < PathFollower.PathPoints.Num() - 1; PathPointIndex++)
			{
				const FRecallPathPoint& StartPathPoint = PathFollower.PathPoints[PathPointIndex];
				const FRecallPathPoint& EndPathPoint = PathFollower.PathPoints[PathPointIndex + 1];

				constexpr double DebugPathOffset = 10.0f;
				const FVector& LineStart = StartPathPoint.Location + FVector::UpVector * DebugPathOffset;
				const FVector& LineEnd = EndPathPoint.Location + FVector::UpVector * DebugPathOffset;
				const FColor& LineColor = PathPointIndex < PathFollower.GetCurrentSegmentIndex() ? FColor::Red : FColor::Green;

				DrawDebugLine(Context.GetWorld(), LineStart, LineEnd, LineColor);
			}
		}
	});
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
